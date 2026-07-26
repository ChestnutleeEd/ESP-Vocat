#include <stdio.h>

#include "esp_idf_version.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    puts("ESP-VoCat PCB V1.0 Smoke Test Candidate");
    printf("Compile-time ESP-IDF version: %d.%d.%d\n",
           ESP_IDF_VERSION_MAJOR,
           ESP_IDF_VERSION_MINOR,
           ESP_IDF_VERSION_PATCH);
    puts("PCB target: ESP-VoCat PCB V1.0");
    puts("Host-built candidate");
    puts("Device execution not yet authorized");
    puts("PSRAM intentionally disabled");
    puts("No peripheral initialization");
    puts("Ready marker: PCB_V1_SMOKE_TEST_CANDIDATE_READY");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
