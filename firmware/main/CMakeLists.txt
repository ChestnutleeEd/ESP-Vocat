idf_component_register(
    SRCS
        "main.c"
        "pcb_v1_display.c"
        "pcb_v1_display_test_pattern.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_driver_gpio
        esp_driver_ledc
        esp_driver_spi
        esp_lcd
        freertos
        heap
)
