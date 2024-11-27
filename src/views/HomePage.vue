<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="py-2 px-4 bg-base-100 w-full flex items-center justify-between">
          <div class="w-full">
          </div>
          <div class="flex items-center gap-3">
            <button class="btn btn-base-300" @click="$router.push({name: 'Home'})">Monitoring</button>
            <button class="btn btn-base-300" @click="$router.push({name: 'Controlling'})">Controlling</button>
          </div>
        </div>
        <div class="grid grid-cols-4 min-h-[88vh] items-center justify-items-center">
          <div class="col-span-4 md:col-span-2 p-4 text-left w-full space-y-2">
            <card-view-vue header="User Information">
              <div class="grid w-full grid-cols-1 md:grid-cols-3 gap-3">
                <div>
                  <label for="name_input">Name</label>
                  <input v-model="user_name" type="text" id="name" placeholder="Name" class="input input-bordered w-full" />
                </div>
                <div>
                  <label for="address_input">Address</label>
                  <input v-model="user_address" type="text" id="address" placeholder="Address" class="input input-bordered w-full" />
                </div>
                <div>
                  <label for="age_input">Age</label>
                  <input v-model="user_age" type="number" id="age" placeholder="Age" class="input input-bordered w-full" />
                </div>
              </div>
              <div class="w-full flex justify-items-end justify-end">
                <button class="btn btn-primary mt-3" @click="onsubmitUser()">Save</button>
              </div>
            </card-view-vue>
            <card-view-vue header="Data Table">
              <div class="flex items-center gap-3 mb-6">
                <button class="btn btn-primary" @click="exportToExcel()">Export Excel</button>
                <button class="btn btn-error" @click="deleteAll()">Delete All</button>
              </div>
              <div class="w-full max-h-[60vh] h-[33vh] overflow-auto">
                <table class="table table-sm">
                  <thead>
                    <tr>
                      <th></th>
                      <th>Ultrasonic 1</th>
                      <th>Ultrasonic 2</th>
                      <th>Ultrasonic 3</th>
                      <th>Ultrasonic 4</th>
                      <th>Classified</th>
                      <th>Timestamp</th>
                      <th>Action</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr v-for="(item, index) in tableData" :key="index">
                      <th>{{ index + 1 }}</th>
                      <td>{{ item.ultrasonic1 }} cm</td>
                      <td>{{ item.ultrasonic2 }} cm</td>
                      <td>{{ item.ultrasonic3 }} cm</td>
                      <td>{{ item.ultrasonic4 }} cm</td>
                      <td>{{ item.classified }}</td>
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
import { IonContent, IonPage } from '@ionic/vue';
import { ref, Ref, onMounted } from 'vue';
import { database, ref as firebaseRef, get } from '@/firebaseConfig';
import { remove, child } from 'firebase/database';
import CardViewVue from '@/components/CardView.vue';
import WavesChartVue from '@/components/WavesChart.vue';
import * as XLSX from 'xlsx'

const selectedWave: Ref<any> = ref(-1);
const tableData: Ref<any> = ref([])

const user_name: Ref<String|null> = ref(null)
const user_address: Ref<String|null> = ref(null)
const user_age: Ref<String|null> = ref(null)

onMounted(() => {
  fetchDataFromFirebase();
  document.documentElement.setAttribute('data-theme', 'pastel')
  const localUser = JSON.parse(sessionStorage.getItem('user') as string)
  if(localUser) {
    user_name.value = localUser.displayName
  }
  const assignedUser = JSON.parse(sessionStorage.getItem('a_user') as string)
  if(assignedUser) {
    user_name.value = assignedUser.name
    user_address.value = assignedUser.address
    user_age.value = assignedUser.age
  }
});

function onsubmitUser() {
  sessionStorage.setItem('a_user', JSON.stringify({
    "name": user_name.value,
    "address": user_address.value,
    "age": user_age.value,
  }))
}

async function fetchDataFromFirebase() {
  try {
    const snapshot = await get(firebaseRef(database, 'ultrasonic_leg/data'));
    if (snapshot.exists()) {
      const data = snapshot.val();
      tableData.value = Object.entries(data).map(([key, value]: any) => ({
        key,
        ultrasonic1: value.ultrasonics[0],
        ultrasonic2: value.ultrasonics[1],
        ultrasonic3: value.ultrasonics[2],
        ultrasonic4: value.ultrasonics[3],
        timestamp: value.timestamp,
        classified: value.classified
      }));
      console.log(tableData.value)
      const ultrasonic1 = [];
      const ultrasonic2 = [];
      const ultrasonic3 = [];
      const ultrasonic4 = [];

      for (const key in data) {
        const entry = data[key];
        const [datePart, timePart] = entry.timestamp.split(" ");
        const [day, month, year] = datePart.split("/");
        const formattedDate = `${year}-${month}-${day}T${timePart}:00`;
        console.log(entry)
        const date = new Date(formattedDate);
        if (!isNaN(date.getTime())) {
          ultrasonic1.push({
              value: entry.ultrasonics[0],
              date: formattedDate
          });
          ultrasonic2.push({
              value: entry.ultrasonics[1],
              date: formattedDate
          });
          ultrasonic3.push({
              value: entry.ultrasonics[2],
              date: formattedDate
          });
          ultrasonic4.push({
              value: entry.ultrasonics[3],
              date: formattedDate
          });
        } else {
            console.error("Invalid date:", entry.timestamp);
        }
      }

      waves.value[0].data = ultrasonic1;
      waves.value[1].data = ultrasonic2;
      waves.value[2].data = ultrasonic3;
      waves.value[3].data = ultrasonic4;
    } else {
      console.log("No data available");
    }   
  } catch (error) {
    console.error("Error fetching data from Firebase:", error);
  }
}

const waves = ref([
  { name: 'Ultrasonic 1', data: [] as { value: number; date: string }[] },
  { name: 'Ultrasonic 2', data: [] as { value: number; date: string }[] },
  { name: 'Ultrasonic 3', data: [] as { value: number; date: string }[] },
  { name: 'Ultrasonic 4', data: [] as { value: number; date: string }[] },
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