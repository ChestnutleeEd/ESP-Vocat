#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PCB_V1_DISPLAY_STATE_BOOT_MARKER = 0,
    PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF,
    PCB_V1_DISPLAY_STATE_QSPI_BUS_INIT,
    PCB_V1_DISPLAY_STATE_PANEL_IO_CREATE,
    PCB_V1_DISPLAY_STATE_PANEL_RESET,
    PCB_V1_DISPLAY_STATE_PANEL_INIT,
    PCB_V1_DISPLAY_STATE_DISPLAY_ON,
    PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
    PCB_V1_DISPLAY_STATE_BACKLIGHT_POLICY_GATE,
    PCB_V1_DISPLAY_STATE_READY,
    PCB_V1_DISPLAY_STATE_FAIL_SAFE,
} pcb_v1_display_state_t;

typedef struct {
    pcb_v1_display_state_t terminal_state;
    pcb_v1_display_state_t failed_state;
    esp_err_t error;
} pcb_v1_display_result_t;

pcb_v1_display_result_t pcb_v1_display_run_once(void);
const char *pcb_v1_display_state_name(pcb_v1_display_state_t state);

#ifdef __cplusplus
}
#endif
