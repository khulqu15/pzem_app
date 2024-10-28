<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="py-2 px-4 bg-base-100 w-full flex items-center justify-between">
          <div class="w-full">
            <h2>Love Bird</h2>
          </div>
          <div class="flex items-center gap-3">
            <button class="btn btn-base-300" @click="$router.push({name: 'Home'})">Home</button>
            <button class="btn btn-base-300" @click="$router.push({name: 'Scheduling'})">Scheduling</button>
          </div>
        </div>
        <div class="grid grid-cols-4 min-h-[88vh] items-center justify-items-center">
          <div class="col-span-4 md:col-span-2 p-4 text-left w-full space-y-2">
            <img src="/public/banner.png" class="w-full shadow-xl rounded-3xl" alt="">
            <card-view-vue header="Data Table">
              <div class="flex items-center gap-3 mb-6">
                <button class="btn btn-primary" @click="exportToExcel()">Export Excel</button>
                <button class="btn btn-error" @click="deleteAll()">Delete All</button>
              </div>
              <div class="w-full max-h-[60vh] h-[20vh] overflow-auto">
                <table class="table table-sm">
                  <thead>
                    <tr>
                      <th></th>
                      <th>Load Cell</th>
                      <th>Water Level</th>
                      <th>Timestamp</th>
                      <th>Action</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr v-for="(item, index) in tableData" :key="index">
                      <th>{{ index + 1 }}</th>
                      <td>{{ item.loadcell }} gram</td>
                      <td>{{ item.water_level }} mm</td>
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
            <div v-if="waves.length > 0">
              <card-view-vue header="Chart Plotting">
                <div v-if="selectedWave == -1">
                  <waves-chart-vue 
                    :wave-data="waves.map(wave => wave.data)" 
                    :wave-names="waves.map(wave => wave.name)" 
                  />
                </div>
                <div v-else>
                  <waves-chart-vue 
                    :wave-data="[waves[selectedWave].data]" 
                    :wave-names="[waves[selectedWave].name]" 
                  />
                </div>
              </card-view-vue>
            </div>
          </div>

        </div>
      </div>
    </ion-content>
  </ion-page>
</template>

<script setup lang="ts">
import CardViewVue from '@/components/CardView.vue';
import { IonContent, IonPage } from '@ionic/vue';
import { ref, Ref, onMounted } from 'vue';
import { database, ref as firebaseRef, get } from '@/firebaseConfig';
import { remove, child } from 'firebase/database';
import WavesChartVue from '@/components/WavesChart.vue';
import * as XLSX from 'xlsx'

const selectedWave: Ref<any> = ref(-1);
const tableData: Ref<any> = ref([])

onMounted(() => {
  fetchDataFromFirebase();
  document.documentElement.setAttribute('data-theme', 'light')
});


async function fetchDataFromFirebase() {
  try {
    const snapshot = await get(firebaseRef(database, 'love_bird/data'));
    if (snapshot.exists()) {
      const data = snapshot.val();
      tableData.value = Object.entries(data).map(([key, value]: any) => ({
        key,
        loadcell: value.load_cell,
        timestamp: value.timestamp,
        water_level: value.water_level
      }));

      const loadCells = [];
      const waterLevels = [];

      for (const key in data) {
        const entry = data[key];
        const [datePart, timePart] = entry.timestamp.split(" ");
        const [day, month, year] = datePart.split("/");
        const formattedDate = `${year}-${month}-${day}T${timePart}:00`;

        const date = new Date(formattedDate);
        if (!isNaN(date.getTime())) {
            loadCells.push({
                value: entry.load_cell,
                date: formattedDate
            });

            waterLevels.push({
                value: entry.water_level,
                date: formattedDate
            });
        } else {
            console.error("Invalid date:", entry.timestamp);
        }
      }

      waves.value[0].data = loadCells;
      waves.value[1].data = waterLevels;
    } else {
      console.log("No data available");
    }
  } catch (error) {
    console.error("Error fetching data from Firebase:", error);
  }
}

const waves = ref([
  { name: 'Load Cell', data: [] as { value: number; date: string }[] },
  { name: 'Water Level', data: [] as { value: number; date: string }[] },
]);


async function deleteByKey(key: string) {
  try {
    await remove(firebaseRef(database, `love_bird/data/${key}`));
    tableData.value = tableData.value.filter((item: any) => item.key !== key);
    console.log(`Entry with key ${key} deleted successfully`);
  } catch (error) {
    console.error(`Error deleting entry with key ${key}:`, error);
  }
}

async function deleteAll() {
  try {
    await remove(firebaseRef(database, 'love_bird/data'));
    tableData.value = [];
    console.log("All entries deleted successfully");
  } catch (error) {
    console.error("Error deleting all entries:", error);
  }
}

function exportToExcel() {
  const waveData = waves.value.map(wave => {
    return {
      name: wave.name,
      data: wave.data.map(point => ({
        value: point.value,
        date: point.date
      }))
    };
  });

  const waveSheet = XLSX.utils.json_to_sheet(
    waveData.flatMap(wave => wave.data.map((point, index) => ({
      Name: wave.name,
      Value: point.value,
      Date: point.date,
      Index: index + 1
    })))
  );

  const workbook = XLSX.utils.book_new();
  XLSX.utils.book_append_sheet(workbook, waveSheet, 'Waves');
  XLSX.writeFile(workbook, 'WaveData.xlsx');
}

</script>