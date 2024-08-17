<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="p-5 bg-base-100 w-full">
          <h2>ECG Generator Simulation</h2>
        </div>
        <input type="checkbox" id="insert_modal" class="modal-toggle" />
        <div role="dialog" class="modal">
          <div class="modal-box">
            <h3 class="text-lg font-bold">Insert Data</h3>
            <p class="pb-4">Fill these inputs first</p>
            <form>
              <div class="form_control">
                <label for="name_input">Data name</label>
                <input v-model="form_input.name" type="text" id="name_input" placeholder="ECG Signal" class="input input-bordered w-full mt-2" />
              </div>
              <div class="flex mt-3 gap-4 justify-end">
                <label for="insert_modal" class="btn bg-base-200 text-white">Cancel</label>
                <label for="insert_modal" @click="insertData()" type="button" class="btn bg-primary text-white">Insert Data</label>
              </div>
            </form>
          </div>
        </div>
        <div class="grid grid-cols-4 min-h-[88vh] items-center justify-items-center">
          <div class="col-span-4 md:col-span-1 p-4 text-left w-full space-y-2">
            <card-view-vue header="Input">
              <div class="flex w-full gap-3 justify-items-center justify-center mb-4">
                <label for="insert_modal" class="btn btn-primary text-white">Insert</label>
                <button @click="clearData()" class="btn btn-error text-white">Clear</button>
              </div>
            </card-view-vue>

            <card-view-vue header="Select R-R Wave">
              <div class="h-64 max-h-[160px] pr-3 overflow-auto">
                <div @click="selectedWave = -1" class="my-1 hover:bg-base-200 py-2 px-3 rounded-xl" :class="{'bg-base-200': selectedWave == -1}">
                  Select All
                </div>
                <div @click="selectedWave = index" class="my-1 hover:bg-base-200 py-2 px-3 rounded-xl flex gap-2" :class="{'bg-base-200': selectedWave == index}" v-for="(item, index) in waves" :key="index">
                  <input v-model="item.name" class="input input-bordered w-full input-xs" />
                  <button @click="deleteWave(index)" class="btn btn-error btn-xs text-white">Delete</button>
                </div>
              </div>
              <button @click="processSignal()" v-if="selectedWave != 1" class="btn btn-primary w-full mt-4 text-white">Process Signal</button>
            </card-view-vue>
          </div>

          <div class="col-span-4 md:col-span-2 p-4 text-left w-full space-y-2">
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

          <div class="col-span-4 md:col-span-1 p-4 text-left w-full space-y-2">
            <card-view-vue header="Output">
              <div class="space-y-3">
                <div class="flex px-4 justify-between items-center">
                  <div>Average</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.average }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Deviation</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.standardDeviation }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Min</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.min }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Max</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.max }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Noise</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.noise }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Amplitude</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.amplitude }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Frequency</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.frequency }}</div>
                </div>
                <div class="flex px-4 justify-between items-center">
                  <div>Phase</div>
                  <div class="font-bold bg-primary px-4 py-1 rounded-xl text-white">{{ outputData.phase }}</div>
                </div>
              </div>
              <div class="flex w-full gap-3 justify-items-center justify-center mt-4">
                <button @click="exportToExcel()" class="btn btn-primary text-white">Convert</button>
                <button @click="clearOutput()" class="btn btn-error text-white">Clear</button>
              </div>
            </card-view-vue>
          </div>
        </div>
      </div>
    </ion-content>
  </ion-page>
</template>

<script setup lang="ts">
import CardViewVue from '@/components/CardView.vue';
import { IonContent, IonPage } from '@ionic/vue';
import { ref, Ref } from 'vue';
import WavesChartVue from '@/components/WavesChart.vue';
import * as XLSX from 'xlsx'

const selectedWave: Ref<any> = ref(-1);
const form_input: Ref<{name: String}> = ref({
  name: ""
})

const outputData: Ref<any> = ref({
  average: 0,
  standardDeviation: 0,
  min: 0,
  max: 0,
  noise: 0,
  amplitude: 0,
  frequency: 0,
  phase: 0
});

type WaveData = {
  value: number;
  date: string;
};

type Wave = {
  name: string;
  data: WaveData[];
};

const waves: Ref<Wave[]> = ref([
  {
    name: "ECG Waves",
    data: Array.from({ length: 10 }, () => ({
      value: generateRandomValue(8, 14),
      date: generateRandomDate(new Date('2024-08-10T10:23:00'), new Date('2024-08-10T10:29:00'))
    }))
  },
  {
    name: "EMG Waves",
    data: Array.from({ length: 10 }, () => ({
      value: generateRandomValue(8, 14),
      date: generateRandomDate(new Date('2024-08-10T10:23:00'), new Date('2024-08-10T10:29:00'))
    }))
  },
  {
    name: "EEG Waves",
    data: Array.from({ length: 10 }, () => ({
      value: generateRandomValue(8, 14),
      date: generateRandomDate(new Date('2024-08-10T10:23:00'), new Date('2024-08-10T10:29:00'))
    }))
  },
  {
    name: "Otot Waves",
    data: Array.from({ length: 10 }, () => ({
      value: generateRandomValue(8, 14),
      date: generateRandomDate(new Date('2024-08-10T10:23:00'), new Date('2024-08-10T10:29:00'))
    }))
  },
  {
    name: "Otak Waves",
    data: Array.from({ length: 10 }, () => ({
      value: generateRandomValue(8, 14),
      date: generateRandomDate(new Date('2024-08-10T10:23:00'), new Date('2024-08-10T10:29:00'))
    }))
  },
])


function generateRandomValue(min: number, max: number): number {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

function generateRandomDate(start: Date, end: Date): string {
  const date = new Date(start.getTime() + Math.random() * (end.getTime() - start.getTime()));
  return date.toISOString().slice(0, 19).replace('T', ' ');
}

function insertData() {
  let temp_data: Wave[] = [{
    name: form_input.value && form_input.value.name ? form_input.value.name.toString() : 'Dummy Data',
    data: Array.from({ length: 10 }, () => ({
      value: generateRandomValue(8, 14),
      date: generateRandomDate(new Date('2024-08-10T10:23:00'), new Date('2024-08-10T10:29:00'))
    }))
  }]

  waves.value.push(...temp_data)
}

function clearData() {
  let temp_data = []
  waves.value = []
}

function processSignal() {
  if (selectedWave.value !== -1) {
    const wave = waves.value[selectedWave.value].data;
    const total = wave.reduce((acc: any, point: { value: any; }) => acc + point.value, 0);
    const average = total / wave.length;

    const deviations = wave.map((point: { value: number; }) => (point.value - average) ** 2);
    const variance = deviations.reduce((acc: any, dev: any) => acc + dev, 0) / wave.length;
    const stdDeviation = Math.sqrt(variance);
    const standardDeviation = stdDeviation.toFixed(2)

    const min = Math.min(...wave.map((point: { value: any; }) => point.value));
    const max = Math.max(...wave.map((point: { value: any; }) => point.value));

    const noise = standardDeviation;
    const amplitude = max - min;
    const frequency = 1;
    const phase = 0;

    outputData.value = {
      average,
      standardDeviation,
      min,
      max,
      noise,
      amplitude,
      frequency,
      phase
    };
  }
}

function clearOutput() {
  outputData.value.average = 0
  outputData.value.standardDeviation = 0
  outputData.value.min = 0
  outputData.value.max = 0
  outputData.value.noise = 0
  outputData.value.amplitude = 0
  outputData.value.frequency = 0
  outputData.value.phase = 0
}

function deleteWave(index: number) {
  waves.value.splice(index, 1);
  if (selectedWave.value === index) selectedWave.value = -1
  else if (selectedWave.value > index) selectedWave.value -= 1
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

  const outputSheet = XLSX.utils.json_to_sheet([
    { Metric: 'Average', Value: outputData.value.average },
    { Metric: 'Standard Deviation', Value: outputData.value.standardDeviation },
    { Metric: 'Min', Value: outputData.value.min },
    { Metric: 'Max', Value: outputData.value.max },
    { Metric: 'Noise', Value: outputData.value.noise },
    { Metric: 'Amplitude', Value: outputData.value.amplitude },
    { Metric: 'Frequency', Value: outputData.value.frequency },
    { Metric: 'Phase', Value: outputData.value.phase }
  ]);

  const workbook = XLSX.utils.book_new();
  XLSX.utils.book_append_sheet(workbook, waveSheet, 'Waves');
  XLSX.utils.book_append_sheet(workbook, outputSheet, 'Output');

  XLSX.writeFile(workbook, 'WaveData.xlsx');
}

</script>