#include <stdio.h>

#include "esp_idf_version.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pcb_v1_display.h"

void app_main(void)
{
    puts("ESP-VoCat PCB V1.0 Minimal Display Smoke Test");
    printf("Compile-time ESP-IDF version: %d.%d.%d\n",
           ESP_IDF_VERSION_MAJOR,
           ESP_IDF_VERSION_MINOR,
           ESP_IDF_VERSION_PATCH);
    puts("PCB target: ESP-VoCat PCB V1.0");
    puts("Host-built candidate");
    puts("Device execution not yet authorized");
    puts("PSRAM intentionally disabled");
    puts("Artifact policy: backlight=hard-disabled visual=UNVERIFIED "
         "not-for-visual-validation device-execution=NOT_AUTHORIZED");

    const pcb_v1_display_result_t result = pcb_v1_display_run_once();
    if (result.terminal_state == PCB_V1_DISPLAY_STATE_READY) {
        puts("LCD_TEST_RESULT READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED");
    } else {
        printf("LCD_TEST_RESULT FAIL state=%s err=%s\n",
               pcb_v1_display_state_name(result.failed_state),
               esp_err_to_name(result.error));
    }

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
