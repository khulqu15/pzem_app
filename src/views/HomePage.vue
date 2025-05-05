<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="fixed top-0 left-0 w-full bg-base-100 p-6 z-[999]">
          <div class="w-full flex items-center justify-between">
            <div>
              <h1 class="flex items-center gap-4 text-lg font-bold text-blue-600"><Icon icon="mynaui:building-solid" width="32" height="32" /></h1>
            </div>
            <div class="flex items-center gap-3">
              <button class="btn" @click="tab = 'ownerData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'ownerData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'ownerData'}">Owner</button>
              <button class="btn" @click="tab = 'userData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'userData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'userData'}">User</button>
              <button class="btn" @click="tab = 'historyData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'historyData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'historyData'}">History</button>
            </div>
          </div>
        </div>
        <div v-if="tab == 'historyData'">
          <div class="mb-4 pt-32 px-8 flex items-center justify-between">
            <div>
              <label for="pzemSelect" class="font-semibold mr-2">Select PZEM Device:</label>
              <select id="pzemSelect" class="select select-bordered" v-model="selectedPzemIndex">
                <option v-for="(entry, index) in tableData" :key="index" :value="index">PZEM {{ index + 1 }}</option>
              </select>
            </div>
            <div>
              <button class="btn bg-blue-600 text-white" @click="exportToExcel()">Export CSV</button>
            </div>
          </div>
          <div class="grid grid-cols-1 lg:grid-cols-2 gap-4 p-4 w-full">
            <card-view-vue :header="`PZEM ${selectedPzemIndex + 1} Data Table`">
              <table class="table table-sm">
                <thead>
                  <tr>
                    <th>#</th>
                    <th>Voltage (V)</th>
                    <th>Current (A)</th>
                    <th>Power (W)</th>
                    <th>Energy (kWh)</th>
                    <th>Timestamp</th>
                  </tr>
                </thead>
                <tbody>
                  <tr v-for="(item, idx) in tableData[selectedPzemIndex]?.data" :key="idx">
                    <td>{{ idx + 1 }}</td>
                    <td>{{ item.voltage }}</td>
                    <td>{{ item.current }}</td>
                    <td>{{ item.power }}</td>
                    <td>{{ item.energy }}</td>
                    <td>{{ item.timestamp }}</td>
                  </tr>
                </tbody>
              </table>
            </card-view-vue>
            <card-view-vue :header="`PZEM ${selectedPzemIndex + 1} Chart`">
            <waves-chart-vue
              :wave-data="[
                voltageData[selectedPzemIndex].values,
                currentData[selectedPzemIndex].values,
                powerData[selectedPzemIndex].values,
                energyData[selectedPzemIndex].values
              ]"
              :wave-names="['Voltage', 'Current', 'Power', 'Energy']"
            />
          </card-view-vue>
          </div>

        </div>
        <div v-if="tab != 'historyData'">
          <div class="w-full relative p-4 flex items-center pt-40 justify-center">
            <div class="max-w-3xl w-full bg-base-100 p-6 rounded-xl">
              <div>
                <div class="flex items-center justify-between">
                  <div class="flex items-center gap-4">
                    <div class="w-16 h-16 flex items-center justify-center bg-blue-600 rounded-2xl text-white">
                      <Icon icon="vaadin:bed" width="25" height="25" />
                    </div>
                    <div>
                      <h1 class="text-xl font-bold">{{ kostInfo.name }}</h1>
                      <p class="text-sm">{{ kostInfo.address }}</p>
                    </div>
                  </div>
                  <div v-if="tab == 'ownerData'">
                    <div class="dropdown dropdown-end">
                      <div role="button" tabindex="0" class="btn bg-base-200">
                        <Icon icon="nimbus:ellipsis" width="16" height="16" />
                      </div>
                      <ul tabindex="0" class="dropdown-content menu bg-base-100 rounded-box z-1 w-52 p-2 shadow-sm">
                        <li><a onclick="edit_kost_modal.showModal()">Edit</a></li>
                      </ul>
                    </div>
                  </div>
                </div>
                <button onclick="form_room_modal.showModal()" class="w-full bg-blue-600 text-white btn mt-6 mb-4 hover:bg-blue-700" v-if="tab == 'ownerData'">Add New</button>
                <dialog id="change_room_modal" class="modal" v-if="tab == 'userData'">
                  <div class="modal-box max-w-2xl">
                    <h3 class="text-lg font-bold">Change Room</h3>
                    <select v-model="form.kost" class="select select-bordered w-full">
                      <option v-for="(kost, idx) in beds" :key="idx" :value="kost.uuid" :disabled="!kost.available" :selected="userInfo.kost == kost.uuid">{{ kost.name }} {{ !kost.available ? '(Unavailable)' : '' }}</option>
                    </select>
                    <div class="modal-action">
                      <form method="dialog">
                        <button class="btn">Close</button>
                        <button @click="userInfo.kost = form.kost" class="btn ml-3 px-12 bg-blue-600 hover:bg-blue-700 text-white">Save</button>
                      </form>
                    </div>
                  </div>
                </dialog>
                <dialog id="edit_kost_modal" class="modal" v-if="tab == 'ownerData'">
                  <div class="modal-box max-w-2xl">
                    <h3 class="text-lg font-bold">Edit Kost Info</h3>
                    <div>
                      <label for="name_input">Name</label>
                      <input type="text" class="input input-bordered w-full" id="name_input" v-model="kostInfo.name">
                    </div>
                    <div class="mt-3">
                      <label for="name_input">Address</label>
                      <input type="text" class="input input-bordered w-full" id="name_input" v-model="kostInfo.address">
                    </div>
                    <div class="modal-action">
                      <form method="dialog">
                        <button class="btn">Close</button>
                        <button @click="userInfo.kost = form.kost" class="btn ml-3 px-12 bg-blue-600 hover:bg-blue-700 text-white">Save</button>
                      </form>
                    </div>
                  </div>
                </dialog>
                <dialog id="form_room_modal" class="modal" v-if="tab == 'ownerData'">
                  <div class="modal-box max-w-2xl">
                    <h3 class="text-lg font-bold">Form Room's Kost</h3>
                    <div class="mt-3">
                      <label for="name_input">Name</label>
                      <input type="text" class="input input-bordered w-full" id="name_input" v-model="kostInfo.name">
                    </div>
                    <div class="modal-action">
                      <form method="dialog">
                        <button class="btn">Close</button>
                        <button @click="userInfo.kost = form.kost" class="btn ml-3 px-12 bg-blue-600 hover:bg-blue-700 text-white">Save</button>
                      </form>
                    </div>
                  </div>
                </dialog>
                <div v-for="(item, index) in beds" :key="index" class="mt-4">
                  <div class="py-4 px-6 hover:bg-base-200 rounded-xl">
                    <div class="flex items-center justify-between">
                      <div class="div">
                        <h3 class="font-bold">{{ item.name }}</h3>
                        <p class="text-sm">{{ item.location }} kWh</p>
                      </div>
                      <div v-if="tab == 'ownerData'">
                        <div class="dropdown dropdown-end">
                          <div role="button" tabindex="0" class="btn bg-base-200">
                            <Icon icon="nimbus:ellipsis" width="16" height="16" />
                          </div>
                          <ul tabindex="0" class="dropdown-content menu bg-base-100 rounded-box z-1 w-52 p-2 shadow-sm">
                            <li><a>Edit</a></li>
                            <li><a>Delete</a></li>
                          </ul>
                        </div>
                      </div>
                      <div v-if="tab == 'userData'" class="flex items-center gap-3">
                        <div class="dropdown dropdown-end">
                          <div role="button" tabindex="0" class="btn bg-base-200">
                            <Icon icon="nimbus:ellipsis" width="16" height="16" />
                          </div>
                          <ul tabindex="0" class="dropdown-content menu bg-base-100 rounded-box z-1 w-52 p-2 shadow-sm">
                            <li><a onclick="change_room_modal.showModal()">Change Room</a></li>
                          </ul>
                        </div>
                        <button class="btn" @click="item.lamp_active = !item.lamp_active" :class="{'bg-blue-600 text-white hover:bg-blue-700': item.lamp_active, 'hover:bg-blue-600 hover:text-white btn-ghost': !item.lamp_active}">
                          <Icon v-if="item.lamp_active" icon="solar:lamp-bold" width="24" height="24" />
                          <Icon v-if="!item.lamp_active" icon="solar:lamp-linear" width="24" height="24" />
                          <span v-if="item.lamp_active">ON</span>
                          <span v-if="!item.lamp_active">OFF</span>
                        </button>
                      </div>
                    </div>
                    <div class="flex mt-5 items-center justify-between gap-5" v-if="item.data">
                      <div>
                        <p class="text-sm text-blue-600 font-semibold">
                          {{ getTotalEnergyAndCost(item.data).totalCost }} ({{ getTotalEnergyAndCost(item.data).totalEnergy }} kWh)
                        </p>
                      </div>
                      <div>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].current }} A</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].voltage }} V</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].power }} W</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].energy }} kWh</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].power_factor }}</span>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
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
import { Icon } from "@iconify/vue";

const tableData : any = ref([]);
const voltageData : any = ref([]);
const currentData : any = ref([]);
const powerData : any = ref([]);
const energyData : any = ref([]);

const tab: any = ref('ownerData')
const kostInfo = ref({
  name: 'Kost Tante Yunia',
  address: 'Bangil, Pasuruan, Jawatimur',
})

const form: any = ref({
  kost: ''
})
const selectedPzemIndex = ref(0);

const userInfo = ref({
  name: 'Yunia Putri Ariesyah',
  kost: '373ufiw3hbf.uidhiandhiabd',
  email: 'yunia23@gmail.id'
})
const beds: any = ref([])

onMounted(() => {
  form.value.kost = userInfo.value.kost
  fetchPzemData();
  document.documentElement.setAttribute('data-theme', 'light');
});

function fetchPzemData() {
  const dataRef = firebaseRef(database, 'pzem/');

  onValue(dataRef, (snapshot) => {
    if (snapshot.exists()) {
      const data = snapshot.val();
      beds.value = data.pzem
      kostInfo.value.name = data.name
      kostInfo.value.address = data.location

      tableData.value = [];
      voltageData.value = [];
      currentData.value = [];
      powerData.value = [];
      energyData.value = [];

      data.pzem.forEach((pzemEntry: any, index: number) => {
        const key = `PZEM ${index + 1}`;
        const deviceData = pzemEntry.data;

        tableData.value.push({ key, data: deviceData });

        voltageData.value.push({
          name: key,
          values: deviceData.map((entry: any) => ({ value: entry.voltage, date: entry.timestamp }))
        });
        currentData.value.push({
          name: key,
          values: deviceData.map((entry: any) => ({ value: entry.current, date: entry.timestamp }))
        });
        powerData.value.push({
          name: key,
          values: deviceData.map((entry: any) => ({ value: entry.power, date: entry.timestamp }))
        });
        energyData.value.push({
          name: key,
          values: deviceData.map((entry: any) => ({ value: entry.energy, date: entry.timestamp }))
        });
      });
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

function getTotalEnergyAndCost(data: any[]) {
  const ratePerKWh = 500;
  const totalEnergy = data.reduce((sum, entry) => sum + (entry.energy || 0), 0);
  const totalCost = totalEnergy * ratePerKWh;

  return {
    totalEnergy: totalEnergy.toFixed(2),
    totalCost: `Rp ${totalCost.toFixed(0).replace(/\B(?=(\d{3})+(?!\d))/g, ".")}`
  };
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
  const selectedData = tableData.value[selectedPzemIndex.value]?.data || [];

  const pzemSheet = XLSX.utils.json_to_sheet(
    selectedData.map((entry: any, index: number) => ({
      Index: index + 1,
      Voltage: entry.voltage,
      Current: entry.current,
      Power: entry.power,
      Energy: entry.energy,
      Timestamp: entry.timestamp,
    }))
  );

  const workbook = XLSX.utils.book_new();
  const sheetName = `PZEM ${selectedPzemIndex.value + 1}`;
  XLSX.utils.book_append_sheet(workbook, pzemSheet, sheetName);
  XLSX.writeFile(workbook, `${sheetName} Data.xlsx`);
}

</script>
