<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="fixed top-0 left-0 w-full bg-base-100 p-6 z-[999]">
          <div class="w-full flex items-center justify-between">
            <div>
              <h1 class="flex items-center gap-4 text-lg font-bold text-blue-600"><Icon icon="mynaui:building-solid" width="32" height="32" /> HAYAKOST</h1>
            </div>
            <div class="flex items-center gap-3">
              <button class="btn" @click="tab = 'ownerData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'ownerData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'ownerData'}">Owner</button>
              <button class="btn" @click="tab = 'userData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'userData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'userData'}">User</button>
              <button class="btn" @click="tab = 'historyData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'historyData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'historyData'}">History</button>
            </div>
          </div>
        </div>
        <div v-if="tab == 'historyData'" class="grid grid-cols-4 min-h-[88vh] items-center justify-items-center">
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
                <div v-for="(item, index) in tab == 'ownerData' ? beds : beds.filter((item: any) => item.uuid == userInfo.kost)" :key="index" class="mt-4">
                  <div class="py-4 px-6 hover:bg-base-200 rounded-xl">
                    <div class="flex items-center justify-between">
                      <div class="div">
                        <h3 class="font-bold">{{ item.name }}</h3>
                        <p class="text-sm">{{ item.voltage }} V, {{ item.ampere }} A, {{ item.power }} kWh</p>
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
                    <div class="bg-base-200 p-6 rounded-xl mt-4" v-if="tab == 'ownerData'">
                      <label :for="`${index}-intensity`">Intensity {{ item.intensity }}</label>
                      <input type="range" v-model="item.intensity" min="0" max="100" value="40" class="range range-primary" />
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

const userInfo = ref({
  name: 'Mohammad Khusnul Khuluq',
  kost: '373ufiw3hbf.uidhiandhiabd',
  email: 'ninno@hayago.id'
})
const beds: any = ref([
  {
    uuid: '373ufiw3hbf.uidhiandhiabd',
    name: 'ROOM VIP',
    power: 120,
    voltage: 221,
    ampere: 20,
    intensity: 100,
    lamp_active: false,
    available: true,
  },
  {
    uuid: '389hu3fb3bf.uifdadnjin',
    name: 'ROOM A1',
    power: 120,
    voltage: 221,
    ampere: 20,
    intensity: 80,
    lamp_active: false,
    available: true,
  },
  {
    uuid: 'jadbjaindja.aduiaibdui3',
    name: 'ROOM A2',
    power: 120,
    voltage: 221,
    ampere: 20,
    lamp_active: false,
    available: true,
  },
  {
    uuid: 'ioehfui3489j.uiefhiwejfn',
    name: 'ROOM A3',
    power: 120,
    voltage: 221,
    ampere: 20,
    intensity: 80,
    lamp_active: false,
    available: true,
  },
  {
    uuid: 'aojdnfie.wifnwijfnjon',
    name: 'ROOM B1',
    power: 120,
    voltage: 221,
    ampere: 20,
    intensity: 70,
    lamp_active: false,
    available: false,
  },
  {
    uuid: 'ajfnowfnojqopfdm.uafhdiaw',
    name: 'ROOM B2',
    power: 120,
    voltage: 221,
    ampere: 20,
    intensity: 70,
    lamp_active: false,
    available: false,
  },
  {
    uuid: 'ajjfnjiaf.fauifbiejn',
    name: 'ROOM B3',
    power: 120,
    voltage: 221,
    ampere: 20,
    intensity: 70,
    lamp_active: false,
    available: true,
  },
])
onMounted(() => {
  form.value.kost = userInfo.value.kost
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
