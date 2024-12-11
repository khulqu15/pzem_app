<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="grid grid-cols-4 min-h-[88vh] items-center justify-items-center">
          <div class="col-span-4 md:col-span-2 p-4 text-left w-full space-y-2">
            <card-view-vue header="PZEM Data Table">
              <div class="flex items-center gap-3 mb-6">
                <button class="btn btn-primary" @click="exportToExcel()">Export Excel</button>
                <button class="btn btn-error" @click="deleteAll()">Delete All</button>
              </div>
              <div class="w-full max-h-[60vh] h-[33vh] overflow-auto">
                <table class="table table-sm">
                  <thead>
                    <tr>
                      <th></th>
                      <th>Voltage (V)</th>
                      <th>Current (A)</th>
                      <th>Power (W)</th>
                      <th>Energy (kWh)</th>
                      <th>Timestamp</th>
                      <th>Actions</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr v-for="(item, index) in tableData" :key="index">
                      <th>{{ index + 1 }}</th>
                      <td>{{ item.voltage }}</td>
                      <td>{{ item.current }}</td>
                      <td>{{ item.power }}</td>
                      <td>{{ item.energy }}</td>
                      <td>{{ item.timestamp }}</td>
                      <td>
                        <button @click="deleteByKey(item.key)" class="btn btn-error btn-sm">Delete</button>
                      </td>
                    </tr>
                  </tbody>
                </table>
              </div>
            </card-view-vue>
          </div>

          <div class="col-span-4 md:col-span-2 p-4 text-left w-full space-y-2">
            <card-view-vue header="PZEM Data Chart">
              <waves-chart-vue 
                :wave-data="[voltageData, currentData, powerData, energyData]" 
                :wave-names="['Voltage', 'Current', 'Power', 'Energy']" 
              />
            </card-view-vue>
          </div>
        </div>
      </div>
    </ion-content>
  </ion-page>
</template>


<script setup lang="ts">
import { IonContent, IonPage } from '@ionic/vue';
import { ref, onMounted } from 'vue';
import { database, ref as firebaseRef, get, remove } from '@/firebaseConfig';
import CardViewVue from '@/components/CardView.vue';
import WavesChartVue from '@/components/WavesChart.vue';
import * as XLSX from 'xlsx';
import { onValue } from 'firebase/database';

const tableData : any = ref([]);
const voltageData : any = ref([]);
const currentData : any = ref([]);
const powerData : any = ref([]);
const energyData : any = ref([]);

onMounted(() => {
  fetchPzemData();
  document.documentElement.setAttribute('data-theme', 'light');
});

function fetchPzemData() {
  const dataRef = firebaseRef(database, 'yunia_pzem/');

  onValue(dataRef, (snapshot) => {
    if (snapshot.exists()) {
      const data = snapshot.val();
      tableData.value = Object.entries(data).map(([key, value]: any) => ({
        key,
        voltage: value.voltage,
        current: value.current,
        power: value.power,
        energy: value.energy,
        timestamp: value.timestamp,
      }));

      voltageData.value = tableData.value.map((entry: any) => ({
        value: entry.voltage,
        date: entry.timestamp,
      }));
      currentData.value = tableData.value.map((entry: any) => ({
        value: entry.current,
        date: entry.timestamp,
      }));
      powerData.value = tableData.value.map((entry: any) => ({
        value: entry.power,
        date: entry.timestamp,
      }));
      energyData.value = tableData.value.map((entry: any) => ({
        value: entry.energy,
        date: entry.timestamp,
      }));

      console.log('PZEM data updated:', tableData.value);
    } else {
      console.log("No data available");
    }
  }, (error) => {
    console.error("Error fetching data from Firebase:", error);
  });
}

async function deleteByKey(key: string) {
  try {
    await remove(firebaseRef(database, `pzem_zigbee/data/${key}`));
    tableData.value = tableData.value.filter((item: any) => item.key !== key);
    console.log(`Entry with key ${key} deleted successfully`);
  } catch (error) {
    console.error(`Error deleting entry with key ${key}:`, error);
  }
}

async function deleteAll() {
  try {
    await remove(firebaseRef(database, 'pzem_zigbee/data'));
    tableData.value = [];
    console.log("All entries deleted successfully");
  } catch (error) {
    console.error("Error deleting all entries:", error);
  }
}

function exportToExcel() {
  const pzemSheet = XLSX.utils.json_to_sheet(
    tableData.value.map((entry: any, index: any) => ({
      Index: index + 1,
      Voltage: entry.voltage,
      Current: entry.current,
      Power: entry.power,
      Energy: entry.energy,
      Timestamp: entry.timestamp,
    }))
  );

  const workbook = XLSX.utils.book_new();
  XLSX.utils.book_append_sheet(workbook, pzemSheet, 'PZEM Data');
  XLSX.writeFile(workbook, 'PzemData.xlsx');
}
</script>
