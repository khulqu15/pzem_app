<template>
    <ion-page>
        <ion-content :fullscreen="true">
            <div class="min-h-screen w-full relative bg-base-200 text-base-content">
                <div class="py-2 px-4 bg-base-100 w-full flex items-center justify-between">
                    <div class="w-full"></div>
                    <div class="flex items-center gap-3">
                        <button class="btn btn-base-300" @click="$router.push({name: 'Home'})">Monitoring</button>
                        <button class="btn btn-base-300" @click="$router.push({name: 'Controlling'})">Controlling</button>
                    </div>
                </div>
                <div class="container p-8 mx-auto">
                    <div class="grid lg:grid-cols-2 md:grid-cols-2 grid-cols-2 gap-3 mt-4">
                        <div v-for="(item, index) in scheduleData" :key="index">
                            <div class="w-full p-6 bg-base-100 rounded-3xl">
                                <div class="grid grid-cols-2 gap-3">
                                    <div>
                                        <h1 class="font-bold text-2xl">Ultrasonic {{ index + 1 }}</h1>
                                        <div class="flex mt-3">
                                            <div>
                                                <label for="duration">Duration (detik)</label>
                                                <input type="number" class="input input-sm input-bordered w-full"
                                                    v-model.number="item.duration"
                                                    @keyup="updateDuration(item.key, item.duration)">
                                            </div>
                                            <div>
                                                <label for="speed">Speed (rpm)</label>
                                                <input type="number" class="input input-sm input-bordered w-full"
                                                    v-model.number="item.speed"
                                                    @keyup="updateSpeed(item.key, item.speed)">
                                            </div>
                                        </div>
                                    </div>
                                    <div class="space-y-2">
                                        <button class="btn btn-primary w-full" @click="updateValue(item.key, item.speed)">Up</button>
                                        <button class="btn btn-primary w-full" @click="updateValue(item.key, -item.speed)">Down</button>
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
import { ref, Ref, onMounted } from 'vue';
import { database, ref as firebaseRef, get, update } from '@/firebaseConfig';

const scheduleData: Ref<any[]> = ref([]);

onMounted(() => {
  fetchDataFromFirebase();
  document.documentElement.setAttribute('data-theme', 'pastel');
});

async function fetchDataFromFirebase() {
  try {
    const snapshot = await get(firebaseRef(database, 'ultrasonic_leg/control'));
    if (snapshot.exists()) {
      const data = snapshot.val();
      scheduleData.value = Object.entries(data).map(([key, value]: any) => ({
        key,
        active: value.active,
        duration: value.duration,
        speed: value.speed,
        value: value.value
      }));
      console.log(scheduleData.value);
    } else {
      console.log("No data available");
    }
  } catch (error) {
    console.error("Error fetching data from Firebase:", error);
  }
}

function updateDuration(key: string, duration: number) {
  try {
    update(firebaseRef(database, `ultrasonic_leg/control/${key}`), { duration });
    console.log(`Updated duration for ${key} to ${duration}`);
  } catch (error) {
    console.error(`Error updating duration for ${key}:`, error);
  }
}

function updateSpeed(key: string, speed: number) {
  try {
    update(firebaseRef(database, `ultrasonic_leg/control/${key}`), { speed });
    console.log(`Updated speed for ${key} to ${speed}`);
  } catch (error) {
    console.error(`Error updating speed for ${key}:`, error);
  }
}

function updateValue(key: string, speed: number) {
  const item = scheduleData.value.find((entry) => entry.key === key);
  if (item) {
    const newValue = item.value + speed;
    try {
      update(firebaseRef(database, `ultrasonic_leg/control/${key}`), { value: newValue });
      item.value = newValue; // Update the local state
      console.log(`Updated value for ${key} to ${newValue}`);
    } catch (error) {
      console.error(`Error updating value for ${key}:`, error);
    }
  }
}
</script>
