<template>
  <ion-page>
    <ion-content :fullscreen="true">
      <div class="min-h-screen w-full relative bg-base-200 text-base-content">
        <div class="py-2 px-4 bg-base-100 w-full flex items-center justify-between">
          <div class="w-full">
            <h2>ECG Generator Simulation</h2>
          </div>
          <div class="flex items-center gap-3">
            <button class="btn btn-base-300" @click="$router.push({name: 'Home'})">Home</button>
            <button class="btn btn-base-300" @click="$router.push({name: 'Simulation'})">Sim</button>
          </div>
        </div>


        <div class="grid grid-cols-4 gap-3 p-8 items-center">
          <div class="lg:col-span-1 col-span-4">
            <div class="p-3 bg-base-100 p-3 rounded-xl space-y-3">
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="window_input" class="capitalize">window <span class="normal-case">(sec)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="window" type="number">
                </div>
                <input v-model="window" id="window_input" type="range" min="0" max="10" class="range" step="0.1" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="isoelectric_input" class="capitalize">isoelectric voltage <span class="normal-case">(mV)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="isoelectric" type="number">
                </div>
                <input v-model="isoelectric" id="isoelectric_input" type="range" min="0" max="1" class="range" step="0.05" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="pqrs_input" class="capitalize">P-QRS Delay <span class="normal-case">(ms)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="pqrs" type="number">
                </div>
                <input v-model="pqrs" id="pqrs_input" type="range" min="0" max="50" class="range" step="0.1" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="qrst_input" class="capitalize">QRS-T Delay <span class="normal-case">(ms)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="qrst" type="number">
                </div>
                <input v-model="qrst" id="qrst_input" type="range" min="0" max="50" class="range" step="0.1" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="bpm_input" class="capitalize">BPM <span class="normal-case">(BPM)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="bpm" type="number">
                </div>
                <input v-model="bpm" id="bpm_input" type="range" min="40" max="140" class="range" step="0.1" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="bpmnoise_input" class="capitalize">BPM Noise <span class="normal-case">(mV)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="bpmnoise" type="number">
                </div>
                <input v-model="bpmnoise" id="bpmnoise_input" type="range" min="0" max="100" class="range" step="0.1" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="whitenoise_input" class="capitalize">White Noise <span class="normal-case">(mV)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="whitenoise" type="number">
                </div>
                <input v-model="whitenoise" id="whitenoise_input" type="range" min="0" max="0.5" class="range" step="0.1" />
              </div>
              <div class="form-control">
                <div class="flex items-center justify-between">
                  <label for="freqnoise_input" class="capitalize">50Hz Noise <span class="normal-case">(mV)</span></label>
                  <input class="input input-bordered input-sm text-sm w-24" v-model="freqnoise" type="number">
                </div>
                <input v-model="freqnoise" id="freqnoise_input" type="range" min="0" max="1" class="range" step="0.1" />
              </div>
              <div>
                <button @click="generateECG()" class="btn btn-primary w-full">Generate</button>
              </div>
            </div>
          </div>
          <div class="lg:col-span-3 col-span-4 p-4">
            <div v-if="plotData.length > 0">
              <waves-chart-vue :key="chartKey" :wave-data="plotData" :wave-names="plotNames" v-if="plotData.length"></waves-chart-vue>
              <div class="bg-base-100 p-4 rounded-lg text-white">
                <h3 class="text-lg mb-2">Output</h3>
                <div class="space-y-2">
                  <div>Average: {{ outputData.average }}</div>
                  <div>Deviation: {{ outputData.standardDeviation }}</div>
                  <div>Min: {{ outputData.min }}</div>
                  <div>Max: {{ outputData.max }}</div>
                  <div>Noise: {{ outputData.noise }}</div>
                  <div>Amplitude: {{ outputData.amplitude }}</div>
                  <div>Frequency: {{ outputData.frequency }}</div>
                  <div>Phase: {{ outputData.phase }}</div>
                </div>
              </div>
              <button class="btn btn-primary mt-3 hover:bg-base-300" @click="exportToExcel()">Export To Excel</button>
            </div>
            <div v-else>
              <div role="alert" class="alert alert-warning">
                <svg
                  xmlns="http://www.w3.org/2000/svg"
                  class="h-6 w-6 shrink-0 stroke-current"
                  fill="none"
                  viewBox="0 0 24 24">
                  <path
                    stroke-linecap="round"
                    stroke-linejoin="round"
                    stroke-width="2"
                    d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
                </svg>
                <span>Please, Generate Data First</span>
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
import { onMounted, Ref, ref } from 'vue';
const window: Ref<number> = ref(5)
const isoelectric: Ref<number> = ref(0.2)
const pqrs: Ref<number> = ref(25)
const qrst: Ref<number> = ref(25)
const bpm: Ref<number> = ref(67)
const bpmnoise: Ref<number> = ref(25)
const whitenoise: Ref<number> = ref(0.2)
const freqnoise: Ref<number> = ref(0.2)
import WavesChartVue from '@/components/WavesChart.vue';
import * as XLSX from 'xlsx'

const plotData = ref<Array<Array<{ value: number; date: string }>>>([]);
const plotNames = ref<string[]>(["ECG Signal"]);

const outputData = ref({
  average: 0,
  standardDeviation: 0,
  min: 0,
  max: 0,
  noise: 0,
  amplitude: 0,
  frequency: bpm.value,
  phase: 0
});
const chartKey = ref(0);

function calculateMetrics(data: number[]) {
  const sum = data.reduce((acc, val) => acc + val, 0);
  const average = sum / data.length;

  const variance = data.reduce((acc, val) => acc + Math.pow(val - average, 2), 0) / data.length;
  const standardDeviation = Math.sqrt(variance);

  const min = Math.min(...data);
  const max = Math.max(...data);
  const amplitude = max - min;

  return {
    average,
    standardDeviation,
    min,
    max,
    noise: standardDeviation,
    amplitude,
    frequency: bpm.value,
    phase: 0
  };
}


onMounted(() => {
  console.log(plotData.value.length)
})

function generateSyntheticECG({
  windowSize = 5,
  isoelectricVoltage = 0.5,
  pQrsDelay = 25,
  qrsTDelay = 25,
  bpm = 60,
  bpmNoise = 0.1,
  whiteNoise = 0.05,
  frequencyNoise = 0.02
}: {
  windowSize: number;
  isoelectricVoltage: number;
  pQrsDelay: number;
  qrsTDelay: number;
  bpm: number;
  bpmNoise: number;
  whiteNoise: number;
  frequencyNoise: number;
}) {
  const sampleRate = 250;
  const numSamples = Math.floor(windowSize * sampleRate);
  const heartRateInterval = (60 / bpm) * sampleRate;

  const signal = Array.from({ length: numSamples }, (_, index) => {
    const time = index / sampleRate;

    const pWave = 0.1 * Math.sin(2 * Math.PI * time * 1.2);
    const qWave = -0.15 * Math.sin(2 * Math.PI * time * 5);
    const rWave = 1.0 * Math.exp(-Math.pow((time % (heartRateInterval / sampleRate)) - 0.1, 2) / 0.002);
    const sWave = -0.2 * Math.sin(2 * Math.PI * time * 10);
    const tWave = 0.3 * Math.sin(2 * Math.PI * time * 0.6);

    let baseSignal = (isNaN(isoelectricVoltage) ? 0 : isoelectricVoltage) + pWave + qWave + rWave + sWave + tWave;

    const bpmNoiseComponent = isNaN(bpmNoise) ? 0 : (Math.random() * 2 - 1) * bpmNoise;
    const whiteNoiseComponent = isNaN(whiteNoise) ? 0 : (Math.random() * 2 - 1) * whiteNoise;
    const frequencyNoiseComponent = isNaN(frequencyNoise) ? 0 : frequencyNoise * Math.sin(2 * Math.PI * 50 * time);

    const totalValue = baseSignal + bpmNoiseComponent + whiteNoiseComponent + frequencyNoiseComponent;
    return isNaN(totalValue) ? 0 : totalValue;
  });

  const formattedData = signal.map((value, index) => ({
    value,
    date: new Date((index / sampleRate) * 1000).toISOString()
  }));

  const metrics = calculateMetrics(signal);
  outputData.value = { ...metrics };

  return [formattedData];
}

function exportToExcel() {
  const dataToExport = plotData.value[0].map((entry) => ({
    Time: entry.date,
    Voltage: entry.value
  }));

  const worksheet = XLSX.utils.json_to_sheet(dataToExport);
  const workbook = XLSX.utils.book_new();
  XLSX.utils.book_append_sheet(workbook, worksheet, 'ECG Data');

  XLSX.writeFile(workbook, 'ECG_Data.xlsx');
}

function generateECG() {
  plotData.value = generateSyntheticECG({
    windowSize: parseFloat(window.value.toString()),
    isoelectricVoltage: parseFloat(isoelectric.value.toString()),
    pQrsDelay: parseFloat(pqrs.value.toString()),
    qrsTDelay: parseFloat(qrst.value.toString()),
    bpm: parseFloat(bpm.value.toString()),
    bpmNoise: parseFloat(bpmnoise.value.toString()),
    whiteNoise: parseFloat(whitenoise.value.toString()),
    frequencyNoise: parseFloat(freqnoise.value.toString())
  });
  chartKey.value += 1;
  console.log(plotData.value)
  console.log(typeof isoelectric.value)
}
</script>
        