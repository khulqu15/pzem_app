<template>
    <div>
        <Line :data="chartData" :options="chartOptions" />
    </div>
</template>

<script lang="ts">
import { defineComponent, computed, PropType } from "vue";
import {
    Chart as Chartjs,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    ChartOptions,
    ChartData,
    Legend,
    Filler
} from 'chart.js';
import { Line } from 'vue-chartjs';
import zoomPlugin from 'chartjs-plugin-zoom';

Chartjs.register(LineElement, PointElement, LinearScale, Title, CategoryScale, Tooltip, Legend, Filler, zoomPlugin);

export default defineComponent({
    name: "WaveChart",
    components: {
        Line,
    },
    props: {
        waveData: {
            type: Array as PropType<Array<Array<{ value: number; date: string }>>>,
            required: true,
        },
        waveNames: {
            type: Array as PropType<Array<string>>,
            required: true,
        }
    },

    setup(props) {
        const colors = ['rgba(75, 192, 192, 1)', 'rgba(255, 99, 132, 1)', 'rgba(54, 162, 235, 1)', 'rgba(255, 206, 86, 1)', 'rgba(153, 102, 255, 1)'];

        const formatDate = (dateString: string) => {
            const date = new Date(dateString);
            const options: Intl.DateTimeFormatOptions = { month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit' };
            return date.toLocaleString('en-US', options);
        };

        const chartData = computed<ChartData<'line'>>(() => {
            return {
                labels: props.waveData[0]?.map(data => formatDate(data.date)) || [],
                datasets: props.waveData.map((data, index) => ({
                    label: props.waveNames[index],
                    data: data.map(d => d.value),
                    borderColor: colors[index % colors.length],
                    backgroundColor: colors[index % colors.length].replace('1)', '0.2)'),
                    fill: true,
                })),
            };
        });

        const chartOptions = computed<ChartOptions<'line'>>(() => ({
            responsive: true,
            plugins: {
                legend: {
                    position: 'top',
                },
                title: {
                    display: true,
                    text: 'Diagram',
                },
                tooltip: {
                    enabled: true,
                },
                zoom: {
                    pan: {
                        enabled: true,
                        mode: 'xy',
                    },
                    zoom: {
                        wheel: {
                            enabled: true,
                        },
                        pinch: {
                            enabled: true,
                        },
                        mode: 'xy',
                    },
                },
            },
            scales: {
                x: {
                    type: 'category',
                    title: {
                        display: true,
                        text: 'Time',
                    },
                },
                y: {
                    type: 'linear',
                    title: {
                        display: true,
                        text: 'Value',
                    },
                    min: -20,
                    max: 20,
                },
            },
        }));

        return {
            chartData,
            chartOptions,
        };
    },
});
</script>
