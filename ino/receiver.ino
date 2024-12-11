#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_zigbee_api_core.h"
#include "firebase_client.h"
#include "firebase_json.h"

#define TAG "ZIGBEE"

const char* db_url = "https://nodell-c25fc-default-rtdb.asia-southeast1.firebasedatabase.app";

FirebaseApp app;
RealtimeDatabase Database;
NoAuth noAuth;

#define ZIGBEE_ENDPOINT 1
#define ZIGBEE_CLUSTER_ID 0x0006

float voltage = 0.0;
float current = 0.0;
float power = 0.0;
float energy = 0.0;

void send_historical_data_to_firebase(float voltage, float current, float power, float energy) {
    char path[128];
    snprintf(path, sizeof(path), "/yunia_pzem");

    FirebaseJson json;
    unsigned long timestamp = esp_timer_get_time() / 1000;
    json.set("timestamp", timestamp);
    json.set("voltage", voltage);
    json.set("current", current);
    json.set("power", power);
    json.set("energy", energy);

    char push_key[128];
    if (Database.push(path, json.to_string().c_str(), push_key, sizeof(push_key))) {
        ESP_LOGI(TAG, "Data added to Firebase successfully with key: %s", push_key);
    } else {
        ESP_LOGE(TAG, "Failed to send data to Firebase: %s", Database.lastErrorMessage());
    }
}

void zigbee_data_received_callback(uint8_t endpoint, uint16_t cluster_id, const uint8_t *data, uint16_t length) {
    if (cluster_id == ZIGBEE_CLUSTER_ID) {
        char received_data[256];
        snprintf(received_data, sizeof(received_data), "%.*s", length, data);
        ESP_LOGI(TAG, "Received Zigbee Data: %s", received_data);

        sscanf(received_data, "%f,%f,%f,%f", &voltage, &current, &power, &energy);

        ESP_LOGI(TAG, "Parsed Voltage: %.2f V, Current: %.2f A, Power: %.2f W, Energy: %.4f kWh",
                 voltage, current, power, energy);

        send_historical_data_to_firebase(voltage, current, power, energy);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing Firebase...");
    initializeApp(app, noAuth);
    app.getApp<RealtimeDatabase>(Database);
    Database.url(db_url);
    ESP_LOGI(TAG, "Firebase initialized!");

    ESP_LOGI(TAG, "Initializing Zigbee...");
    esp_zb_init();
    esp_zb_register_endpoint(ZIGBEE_ENDPOINT, zigbee_data_received_callback);
    esp_zb_start();

    ESP_LOGI(TAG, "Zigbee initialized! Waiting for data...");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

