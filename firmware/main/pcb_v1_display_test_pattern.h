#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    PCB_V1_LCD_WIDTH = 360,
    PCB_V1_LCD_HEIGHT = 360,
    PCB_V1_LCD_STRIP_HEIGHT = 80,
    PCB_V1_LCD_BYTES_PER_PIXEL = 2,
    PCB_V1_LCD_STRIP_BYTES =
        PCB_V1_LCD_WIDTH * PCB_V1_LCD_STRIP_HEIGHT * PCB_V1_LCD_BYTES_PER_PIXEL,
};

void pcb_v1_display_pattern_fill_strip(uint8_t *buffer,
                                       size_t buffer_bytes,
                                       uint16_t y_start,
                                       uint16_t height);

#ifdef __cplusplus
}
#endif
