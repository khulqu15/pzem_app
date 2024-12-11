#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_zigbee_api_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "ZIGBEE"
#define UART_NUM UART_NUM_1
#define TXD_PIN 17
#define RXD_PIN 16
#define PZEM_BAUDRATE 9600
#define BUF_SIZE 256

float voltage = 0.0;
float current = 0.0;
float power = 0.0;
float energy = 0.0;

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;

void init_uart() {
    const uart_config_t uart_config = {
        .baud_rate = PZEM_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}

bool read_pzem_data() {
    uint8_t request[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB};
    uint8_t response[BUF_SIZE];

    uart_flush(UART_NUM);
    uart_write_bytes(UART_NUM, (const char *)request, sizeof(request));
    int len = uart_read_bytes(UART_NUM, response, BUF_SIZE, pdMS_TO_TICKS(100));

    if (len < 7) {
        ESP_LOGE(TAG, "Failed to receive valid data from PZEM");
        return false;
    }

    voltage = ((response[3] << 8) | response[4]) / 10.0;
    current = ((response[5] << 8) | response[6]) / 100.0;
    power = ((response[7] << 8) | response[8]) / 10.0;
    energy = ((response[9] << 8) | response[10]) / 1000.0;

    return true;
}


void send_zigbee_message(const char *message) {
    esp_err_t err = esp_zb_send_data(1, (uint8_t *)message, strlen(message));
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Message sent: %s", message);
    } else {
        ESP_LOGE(TAG, "Failed to send message: %s", esp_err_to_name(err));
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Initializing Zigbee...");

    esp_zb_init();
    esp_zb_start();

    ESP_LOGI(TAG, "Zigbee initialized!");

    while (1) {
        if (xTaskGetTickCount() * portTICK_PERIOD_MS - lastUpdate > updateInterval) {
            lastUpdate = xTaskGetTickCount() * portTICK_PERIOD_MS;

            if (read_pzem_data()) {
                char message[128];
                snprintf(message, sizeof(message),
                         "Voltage: %.2f V, Current: %.2f A, Power: %.2f W, Energy: %.4f kWh",
                         voltage, current, power, energy);

                send_zigbee_message(message);

                ESP_LOGI(TAG, "Data Sent: %s", message);
            } else {
                ESP_LOGE(TAG, "Failed to read data from PZEM");
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
