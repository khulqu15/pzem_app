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
              <button class="btn bg-blue-600 text-white opacity-0" @click="startSimulation()">P</button>
              <button class="btn bg-blue-600 text-white opacity-0" @click="stopSimulation()">S</button>
              <button class="btn" @click="tab = 'ownerData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'ownerData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'ownerData'}">Pemilik</button>
              <button class="btn" @click="tab = 'userData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'userData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'userData'}">Penyewa</button>
              <button class="btn" @click="tab = 'historyData'" :class="{'bg-blue-600 text-white hover:bg-blue-700': tab == 'historyData', 'hover:bg-blue-600 hover:text-white btn-ghost': tab != 'historyData'}">History</button>
              <button class="btn" onclick="my_modal_2.showModal()">Setting</button>
            </div>
          </div>
        </div>
        <dialog id="my_modal_2" class="modal">
          <div class="modal-box">
            <form method="dialog">
              <button class="btn btn-sm btn-circle btn-ghost absolute right-2 top-2">✕</button>
            </form>
            <h3 class="text-lg font-bold">Setting</h3>
            <p class="py-4">Interval Data</p>
            <input type="text" @keyup="setDataInterval()" placeholder="Type here" class="input input-bordered w-full" v-model="interval_data" />
            <form method="dialog" class="mt-2 flex justify-end">
              <button class="btn btn-primary text-white">Save</button>
            </form>
          </div>
          <form method="dialog" class="modal-backdrop">
            <button>close</button>
          </form>
        </dialog>
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
                    <th>
                      #
                    </th>
                    <th>Cost (Rp)</th>
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
                    <td>{{ calculateCost(item.energy) }}</td>
                    <td>{{ item.voltage }}</td>
                    <td>{{ item.current }}</td>
                    <td>{{ item.power }}</td>
                    <td>{{ item.energy }}</td>
                    <td>{{ item.timestamp }}</td>
                  </tr>
                  <tr>
                    <td>-</td>
                    <td>{{ getTotalEnergyAndCost(tableData[selectedPzemIndex]?.data).totalCost }}</td>
                    <td>-</td>
                    <td>-</td>
                    <td>-</td>
                    <td>{{ getTotalEnergyAndCost(tableData[selectedPzemIndex]?.data).totalEnergy }}</td>
                    <td>-</td>
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
                  <div>
                    Total All Item
                    <h1 class="text-xl font-bold text-blue-600">{{ getTotalCostAllPzem() }}</h1>
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
                <button onclick="device_modal.showModal()" class="w-full bg-blue-600 text-white btn mt-6 mb-4 hover:bg-blue-700" v-if="tab == 'ownerData'">Add New</button>
                <dialog id="edit_kost_modal" class="modal" v-if="tab == 'ownerData'">
                  <div class="modal-box max-w-2xl">
                    <h3 class="text-lg font-bold">Edit Kost Info</h3>
                    <div>
                      <label for="name_input">Name</label>
                      <input type="text" class="input input-bordered w-full" id="name_input" v-model="kostInfo.name">
                    </div>
                    <div class="mt-3">
                      <label for="name_input">Location</label>
                      <input type="text" class="input input-bordered w-full" id="name_input" v-model="kostInfo.address">
                    </div>
                    <div class="modal-action">
                      <form method="dialog">
                        <button class="btn">Close</button>
                        <button class="btn ml-3 px-12 bg-blue-600 hover:bg-blue-700 text-white" @click="saveDataParent">Save</button>
                      </form>
                    </div>
                  </div>
                </dialog>
                <dialog id="device_modal" class="modal">
                  <div class="modal-box">
                    <h3 class="text-lg font-bold">{{ isEditing ? 'Edit Device' : 'Create Device' }}</h3>
                    <input type="text" v-model="form.name" placeholder="Device Name" class="input input-bordered w-full mt-3" />
                    <input type="text" v-model="form.location" placeholder="Location" class="input input-bordered w-full mt-3" />
                    <form method="dialog" class="modal-action">
                      <button class="btn" id="close_device_modal">Close</button>
                      <button class="btn bg-blue-600 text-white" @click="saveDataDevice($event), isEditing = false, editingIndex = null">Save</button>
                    </form>
                  </div>
                </dialog>
                <dialog id="delete_device_modal" class="modal">
                  <div class="modal-box">
                    <h3 class="text-lg font-bold text-red-600">Delete Device</h3>
                    <p>Are you sure you want to delete this device?</p>
                    <p class="font-semibold mt-2 text-gray-700">{{ form.name }} - {{ form.location }}</p>
                    <form method="dialog">
                      <button class="btn">Cancel</button>
                      <button class="btn bg-red-600 text-white" @click="confirmDeleteDevice">Delete</button>
                    </form>
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
                            <li><a onclick="device_modal.showModal()" @click="isEditing = true, editingIndex = index, form.name = item.name, form.location = item.location">Edit</a></li>
                            <li><a onclick="delete_device_modal.showModal()" @click="editingIndex = index">Delete</a></li>
                          </ul>
                        </div>
                      </div>
                      <div v-if="tab == 'userData'" class="flex items-center gap-3">
                        <button class="btn" 
                                @click="toggleLamp(item, index)" 
                                :class="{'bg-blue-600 text-white hover:bg-blue-700': item.lamp, 'hover:bg-blue-600 hover:text-white btn-ghost': !item.lamp}">
                            <Icon v-if="item.lamp" icon="solar:lamp-bold" width="24" height="24" />
                            <Icon v-if="!item.lamp" icon="solar:lamp-linear" width="24" height="24" />
                            <span v-if="item.lamp">ON</span>
                            <span v-if="!item.lamp  ">OFF</span>
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
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].current.toFixed(2) }} A</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].voltage.toFixed(2) }} V</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].power.toFixed(2) }} W</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].energy.toFixed(2) }} kWh</span>
                        <span class="badge bg-blue-600 text-sm text-white py-3">{{ item.data[item.data.length - 1].power_factor.toFixed(2) }}</span>
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
import { onValue, set, update } from 'firebase/database';
import { Icon } from "@iconify/vue";

interface PzemData {
  current: number;
  energy: number;
  frequency: number;
  power: number;
  power_factor: number;
  timestamp: string;
  voltage: number;
}

interface EndDevice {
  name: string;
  location: string;
  data: PzemData[];
}

interface KostData {
  name: string;
  location: string;
  pzem: EndDevice[];
}

const interval_data = ref(2000)
const devices = ref<EndDevice[]>([]);
const form = ref<EndDevice>({ name: '', location: '', data: []});
const isEditing = ref(false);
let editingIndex = ref<number | null>(null);
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

const pzemData: KostData = {
  name: "Kost Surabaya",
  location: "Mulyosari Surabaya",
  pzem: [
    {
      name: "End Device 1",
      location: "Kamar 1",
      data: [
        {
          current: 1.13,
          energy: 1.58,
          frequency: 49.98,
          power: 146.84,
          power_factor: 0.91,
          timestamp: "2025-05-06 20:05",
          voltage: 219.81,
        },
        {
          current: 1.56,
          energy: 1.42,
          frequency: 50.27,
          power: 233.01,
          power_factor: 0.92,
          timestamp: "2025-05-06 20:10",
          voltage: 222.98,
        },
      ],
    },
    {
      name: "End Device 2",
      location: "Kamar 2",
      data: [
        {
          current: 1.61,
          energy: 1.86,
          frequency: 50.21,
          power: 373.29,
          power_factor: 0.94,
          timestamp: "2025-05-06 20:05",
          voltage: 220.37,
        },
        {
          current: 0.98,
          energy: 1.63,
          frequency: 50.05,
          power: 311.83,
          power_factor: 0.75,
          timestamp: "2025-05-06 20:10",
          voltage: 224.82,
        },
      ],
    },
    {
      name: "End Device 3",
      location: "Kamar 3",
      data: [
        {
          current: 1.61,
          energy: 1.86,
          frequency: 50.21,
          power: 373.29,
          power_factor: 0.94,
          timestamp: "2025-05-06 20:05",
          voltage: 220.37,
        },
        {
          current: 0.98,
          energy: 1.63,
          frequency: 50.05,
          power: 311.83,
          power_factor: 0.75,
          timestamp: "2025-05-06 20:10",
          voltage: 224.82,
        },
      ],
    },
  ],
};

const isSimulating = ref(false);
let simulationInterval: any;

// Base Values for Stable Random Data
const baseVoltage = 220.0;
const baseCurrent = 1.5;
const basePower = 330.0;
let baseEnergy = 10.0;

function startSimulation() {
  if (isSimulating.value) return;
  isSimulating.value = true;

  simulationInterval = setInterval(() => {
    for (let i = 0; i < 2; i++) {
      addSimulatedData(i);
    }
  }, 2000);
}


function stopSimulation() {
  clearInterval(simulationInterval);
  isSimulating.value = false;
}

// Function to Add Simulated Data to Firebase
async function addSimulatedData(deviceIndex: number) {
  const voltage = baseVoltage + (baseVoltage * ((Math.random() * 0.1) - 0.05)); // ±5%
  const current = baseCurrent + (baseCurrent * ((Math.random() * 0.1) - 0.05)); // ±5%
  const power = voltage * current;
  baseEnergy += power / 3600.0;

  const newData = {
    voltage: parseFloat(voltage.toFixed(2)),
    current: parseFloat(current.toFixed(2)),
    power: parseFloat(power.toFixed(2)),
    energy: parseFloat(baseEnergy.toFixed(3)),
    frequency: 50.0 + (Math.random() * 0.2 - 0.1), // ±0.1 Hz
    power_factor: 0.9 + (Math.random() * 0.1 - 0.05), // 0.85 - 0.95
    timestamp: new Date().toISOString()
  };

  try {
    // Simulate data for /pzem/pzem/[0, 1]/data
    const deviceDataRef = firebaseRef(database, `/pzem/pzem/${deviceIndex}/data`);
    const snapshot = await get(deviceDataRef);

    if (snapshot.exists()) {
      const data = snapshot.val();
      data.push(newData); // Add new data to the end

      await set(deviceDataRef, data);
    } else {
      await set(deviceDataRef, [newData]);
    }

    console.log(`Simulated Data Added to Device ${deviceIndex}:`, newData);
  } catch (error) {
    console.error("Error adding simulated data:", error);
  }
}

function getTotalCostAllPzem() {
  if (!devices.value.length) return "Rp 0";

  const totalCost = devices.value.reduce((total, device) => {
    const deviceCost = device.data.reduce((sum, entry) => sum + (entry.energy || 0) * ratePerKWh, 0);
    return total + deviceCost;
  }, 0);

  return `Rp ${totalCost.toFixed(0).replace(/\B(?=(\d{3})+(?!\d))/g, ".")}`;
}

async function confirmDeleteDevice() {
  if (editingIndex.value !== null) {
    try {
      await remove(firebaseRef(database, `/pzem/pzem/${editingIndex.value}`));
      devices.value.splice(editingIndex.value, 1);
      console.log(`Device at index ${editingIndex.value} deleted successfully.`);
      fetchPzemData();
    } catch (error) {
      console.error("Error deleting device:", error);
    }
  }
}
const ratePerKWh = 500;

function setDataInterval() {
  set(firebaseRef(database, '/pzem/interval'), interval_data.value)
}

function calculateCost(energy: number): string {
  if (!energy) return "Rp 0";
  const cost = energy * ratePerKWh;
  return `Rp ${cost.toFixed(0).replace(/\B(?=(\d{3})+(?!\d))/g, ".")}`;
}

async function saveDataDevice(event: Event) {
  event.preventDefault();

  const initialData = [
    {
      voltage: 0,
      current: 0,
      power: 0,
      energy: 0,
      frequency: 0,
      power_factor: 0,
      timestamp: new Date().toISOString()
    }
  ];

  try {
    if (isEditing.value && editingIndex.value !== null) {
      devices.value[editingIndex.value] = {
        name: form.value.name,
        location: form.value.location,
        data: initialData
      };

      await update(firebaseRef(database, `/pzem/pzem/${editingIndex.value}`), {
        name: form.value.name,
        location: form.value.location,
        data: initialData
      });
      console.log("Device updated successfully.");
    } else {
      const newIndex = devices.value.length;
      devices.value[newIndex] = {
        name: form.value.name,
        location: form.value.location,
        data: initialData
      };

      await set(firebaseRef(database, `/pzem/pzem/${newIndex}`), {
        name: form.value.name,
        location: form.value.location,
        data: initialData
      });
      console.log("New device created successfully.");
    }

    await fetchPzemData();
    clearForm();
    closeDeviceModal();
  } catch (error) {
    console.error("Error saving device data:", error);
  }
}

function clearForm() {
  form.value.name = '';
  form.value.location = '';
  form.value.data = [];
  isEditing.value = false;
  editingIndex.value = null;
}

function closeDeviceModal() {
  const modal = document.getElementById('device_modal') as HTMLDialogElement;
  if (modal) modal.close();
}

async function saveDataParent(event: Event) {
  event.preventDefault();
  try {
    await set(firebaseRef(database, '/pzem/name'), kostInfo.value.name);
    await set(firebaseRef(database, '/pzem/location'), kostInfo.value.address);
    const modal = document.getElementById('edit_kost_modal') as HTMLDialogElement;
    if (modal) modal.close();
  } catch (error) {
    console.error("Error saving Kost info:", error);
  }
}

const selectedPzemIndex = ref(0);

const userInfo = ref({
  name: 'Yunia Putri Ariesyah',
  kost: '373ufiw3hbf.uidhiandhiabd',
  email: 'yunia23@gmail.id'
})
const beds: any = ref([])

onMounted(() => {
  fetchPzemData();
  document.documentElement.setAttribute('data-theme', 'light');
});

async function savePzemData(data: KostData) {
  try {
    await set(firebaseRef(database, "/pzem/pzem"), data);
    console.log("PZEM data saved successfully.");
  } catch (error) {
    console.error("Error saving PZEM data:", error);
  }
}

async function toggleLamp(item: any, index: number) {
  try {
    const toggleValue = !item.lamp
    beds.value[index].lamp = toggleValue
    await set(firebaseRef(database, `/pzem/pzem/${index}/lamp/`), toggleValue);
    fetchPzemData()
  } catch (error) {
    console.error("Error toggling lamp state:", error);
  }
}

function fetchPzemData() {
  const dataRef = firebaseRef(database, 'pzem/');

  onValue(dataRef, (snapshot) => {
    if (snapshot.exists()) {
      const data = snapshot.val();
      beds.value = data.pzem
      kostInfo.value.name = data.name
      kostInfo.value.address = data.location
      interval_data.value = data.interval
      tableData.value = [];
      voltageData.value = [];
      currentData.value = [];
      powerData.value = [];
      energyData.value = [];
      devices.value = data.pzem

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
      devices.value = []
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
