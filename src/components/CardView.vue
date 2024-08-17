<template>
    <div class="w-full relative overflow-hidden rounded-xl bg-base-100">
        <div class="w-full bg-primary py-2 px-4 text-white">{{ header }}</div>
        <div class="p-3">
            <slot></slot>
        </div>
    </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from "vue"

export default defineComponent ({
    props: {
        header: {
            type: String as PropType<string>,
            required: true,
        }
    }
})
</script>