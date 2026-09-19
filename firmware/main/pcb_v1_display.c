#include "pcb_v1_display.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_st77916.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "pcb_v1_display_test_pattern.h"

#if defined(PCB_V1_DISPLAY_HOST_FAULT_INJECTION)
#define HOST_FAULT_POINT(point) pcb_v1_display_host_set_fault_point((point))
#else
#define HOST_FAULT_POINT(point) ((void)0)
#endif

enum {
    LCD_GPIO_CLOCK = GPIO_NUM_18,
    LCD_GPIO_CHIP_SELECT = GPIO_NUM_14,
    LCD_GPIO_DATA_0 = GPIO_NUM_46,
    LCD_GPIO_DATA_1 = GPIO_NUM_13,
    LCD_GPIO_DATA_2 = GPIO_NUM_11,
    LCD_GPIO_DATA_3 = GPIO_NUM_12,
    LCD_GPIO_RESET = GPIO_NUM_3,
    LCD_GPIO_BACKLIGHT = GPIO_NUM_44,
    LCD_PIXEL_CLOCK_HZ = 40000000,
    LCD_TRANSFER_QUEUE_DEPTH = 1,
    LCD_TRANSFER_TIMEOUT_MS = 1000,
};

static const bool LCD_MIRROR_X = false;
static const bool LCD_MIRROR_Y = false;
static const bool LCD_SWAP_XY = false;

#if defined(PCB_V1_DISPLAY_HOST_TABLE_STUB)
static const st77916_lcd_init_cmd_t vendor_specific_init_yysj[184] = {0};
#else
static const st77916_lcd_init_cmd_t vendor_specific_init_yysj[] = {
    {0xF0, (uint8_t []){0x28}, 1, 0},
    {0xF2, (uint8_t []){0x28}, 1, 0},
    {0x73, (uint8_t []){0xF0}, 1, 0},
    {0x7C, (uint8_t []){0xD1}, 1, 0},
    {0x83, (uint8_t []){0xE0}, 1, 0},
    {0x84, (uint8_t []){0x61}, 1, 0},
    {0xF2, (uint8_t []){0x82}, 1, 0},
    {0xF0, (uint8_t []){0x00}, 1, 0},
    {0xF0, (uint8_t []){0x01}, 1, 0},
    {0xF1, (uint8_t []){0x01}, 1, 0},
    {0xB0, (uint8_t []){0x56}, 1, 0},
    {0xB1, (uint8_t []){0x4D}, 1, 0},
    {0xB2, (uint8_t []){0x24}, 1, 0},
    {0xB4, (uint8_t []){0x87}, 1, 0},
    {0xB5, (uint8_t []){0x44}, 1, 0},
    {0xB6, (uint8_t []){0x8B}, 1, 0},
    {0xB7, (uint8_t []){0x40}, 1, 0},
    {0xB8, (uint8_t []){0x86}, 1, 0},
    {0xBA, (uint8_t []){0x00}, 1, 0},
    {0xBB, (uint8_t []){0x08}, 1, 0},
    {0xBC, (uint8_t []){0x08}, 1, 0},
    {0xBD, (uint8_t []){0x00}, 1, 0},
    {0xC0, (uint8_t []){0x80}, 1, 0},
    {0xC1, (uint8_t []){0x10}, 1, 0},
    {0xC2, (uint8_t []){0x37}, 1, 0},
    {0xC3, (uint8_t []){0x80}, 1, 0},
    {0xC4, (uint8_t []){0x10}, 1, 0},
    {0xC5, (uint8_t []){0x37}, 1, 0},
    {0xC6, (uint8_t []){0xA9}, 1, 0},
    {0xC7, (uint8_t []){0x41}, 1, 0},
    {0xC8, (uint8_t []){0x01}, 1, 0},
    {0xC9, (uint8_t []){0xA9}, 1, 0},
    {0xCA, (uint8_t []){0x41}, 1, 0},
    {0xCB, (uint8_t []){0x01}, 1, 0},
    {0xD0, (uint8_t []){0x91}, 1, 0},
    {0xD1, (uint8_t []){0x68}, 1, 0},
    {0xD2, (uint8_t []){0x68}, 1, 0},
    {0xF5, (uint8_t []){0x00, 0xA5}, 2, 0},
    {0xDD, (uint8_t []){0x4F}, 1, 0},
    {0xDE, (uint8_t []){0x4F}, 1, 0},
    {0xF1, (uint8_t []){0x10}, 1, 0},
    {0xF0, (uint8_t []){0x00}, 1, 0},
    {0xF0, (uint8_t []){0x02}, 1, 0},
    {0xE0, (uint8_t []){0xF0, 0x0A, 0x10, 0x09, 0x09, 0x36, 0x35, 0x33, 0x4A, 0x29, 0x15, 0x15, 0x2E, 0x34}, 14, 0},
    {0xE1, (uint8_t []){0xF0, 0x0A, 0x0F, 0x08, 0x08, 0x05, 0x34, 0x33, 0x4A, 0x39, 0x15, 0x15, 0x2D, 0x33}, 14, 0},
    {0xF0, (uint8_t []){0x10}, 1, 0},
    {0xF3, (uint8_t []){0x10}, 1, 0},
    {0xE0, (uint8_t []){0x07}, 1, 0},
    {0xE1, (uint8_t []){0x00}, 1, 0},
    {0xE2, (uint8_t []){0x00}, 1, 0},
    {0xE3, (uint8_t []){0x00}, 1, 0},
    {0xE4, (uint8_t []){0xE0}, 1, 0},
    {0xE5, (uint8_t []){0x06}, 1, 0},
    {0xE6, (uint8_t []){0x21}, 1, 0},
    {0xE7, (uint8_t []){0x01}, 1, 0},
    {0xE8, (uint8_t []){0x05}, 1, 0},
    {0xE9, (uint8_t []){0x02}, 1, 0},
    {0xEA, (uint8_t []){0xDA}, 1, 0},
    {0xEB, (uint8_t []){0x00}, 1, 0},
    {0xEC, (uint8_t []){0x00}, 1, 0},
    {0xED, (uint8_t []){0x0F}, 1, 0},
    {0xEE, (uint8_t []){0x00}, 1, 0},
    {0xEF, (uint8_t []){0x00}, 1, 0},
    {0xF8, (uint8_t []){0x00}, 1, 0},
    {0xF9, (uint8_t []){0x00}, 1, 0},
    {0xFA, (uint8_t []){0x00}, 1, 0},
    {0xFB, (uint8_t []){0x00}, 1, 0},
    {0xFC, (uint8_t []){0x00}, 1, 0},
    {0xFD, (uint8_t []){0x00}, 1, 0},
    {0xFE, (uint8_t []){0x00}, 1, 0},
    {0xFF, (uint8_t []){0x00}, 1, 0},
    {0x60, (uint8_t []){0x40}, 1, 0},
    {0x61, (uint8_t []){0x04}, 1, 0},
    {0x62, (uint8_t []){0x00}, 1, 0},
    {0x63, (uint8_t []){0x42}, 1, 0},
    {0x64, (uint8_t []){0xD9}, 1, 0},
    {0x65, (uint8_t []){0x00}, 1, 0},
    {0x66, (uint8_t []){0x00}, 1, 0},
    {0x67, (uint8_t []){0x00}, 1, 0},
    {0x68, (uint8_t []){0x00}, 1, 0},
    {0x69, (uint8_t []){0x00}, 1, 0},
    {0x6A, (uint8_t []){0x00}, 1, 0},
    {0x6B, (uint8_t []){0x00}, 1, 0},
    {0x70, (uint8_t []){0x40}, 1, 0},
    {0x71, (uint8_t []){0x03}, 1, 0},
    {0x72, (uint8_t []){0x00}, 1, 0},
    {0x73, (uint8_t []){0x42}, 1, 0},
    {0x74, (uint8_t []){0xD8}, 1, 0},
    {0x75, (uint8_t []){0x00}, 1, 0},
    {0x76, (uint8_t []){0x00}, 1, 0},
    {0x77, (uint8_t []){0x00}, 1, 0},
    {0x78, (uint8_t []){0x00}, 1, 0},
    {0x79, (uint8_t []){0x00}, 1, 0},
    {0x7A, (uint8_t []){0x00}, 1, 0},
    {0x7B, (uint8_t []){0x00}, 1, 0},
    {0x80, (uint8_t []){0x48}, 1, 0},
    {0x81, (uint8_t []){0x00}, 1, 0},
    {0x82, (uint8_t []){0x06}, 1, 0},
    {0x83, (uint8_t []){0x02}, 1, 0},
    {0x84, (uint8_t []){0xD6}, 1, 0},
    {0x85, (uint8_t []){0x04}, 1, 0},
    {0x86, (uint8_t []){0x00}, 1, 0},
    {0x87, (uint8_t []){0x00}, 1, 0},
    {0x88, (uint8_t []){0x48}, 1, 0},
    {0x89, (uint8_t []){0x00}, 1, 0},
    {0x8A, (uint8_t []){0x08}, 1, 0},
    {0x8B, (uint8_t []){0x02}, 1, 0},
    {0x8C, (uint8_t []){0xD8}, 1, 0},
    {0x8D, (uint8_t []){0x04}, 1, 0},
    {0x8E, (uint8_t []){0x00}, 1, 0},
    {0x8F, (uint8_t []){0x00}, 1, 0},
    {0x90, (uint8_t []){0x48}, 1, 0},
    {0x91, (uint8_t []){0x00}, 1, 0},
    {0x92, (uint8_t []){0x0A}, 1, 0},
    {0x93, (uint8_t []){0x02}, 1, 0},
    {0x94, (uint8_t []){0xDA}, 1, 0},
    {0x95, (uint8_t []){0x04}, 1, 0},
    {0x96, (uint8_t []){0x00}, 1, 0},
    {0x97, (uint8_t []){0x00}, 1, 0},
    {0x98, (uint8_t []){0x48}, 1, 0},
    {0x99, (uint8_t []){0x00}, 1, 0},
    {0x9A, (uint8_t []){0x0C}, 1, 0},
    {0x9B, (uint8_t []){0x02}, 1, 0},
    {0x9C, (uint8_t []){0xDC}, 1, 0},
    {0x9D, (uint8_t []){0x04}, 1, 0},
    {0x9E, (uint8_t []){0x00}, 1, 0},
    {0x9F, (uint8_t []){0x00}, 1, 0},
    {0xA0, (uint8_t []){0x48}, 1, 0},
    {0xA1, (uint8_t []){0x00}, 1, 0},
    {0xA2, (uint8_t []){0x05}, 1, 0},
    {0xA3, (uint8_t []){0x02}, 1, 0},
    {0xA4, (uint8_t []){0xD5}, 1, 0},
    {0xA5, (uint8_t []){0x04}, 1, 0},
    {0xA6, (uint8_t []){0x00}, 1, 0},
    {0xA7, (uint8_t []){0x00}, 1, 0},
    {0xA8, (uint8_t []){0x48}, 1, 0},
    {0xA9, (uint8_t []){0x00}, 1, 0},
    {0xAA, (uint8_t []){0x07}, 1, 0},
    {0xAB, (uint8_t []){0x02}, 1, 0},
    {0xAC, (uint8_t []){0xD7}, 1, 0},
    {0xAD, (uint8_t []){0x04}, 1, 0},
    {0xAE, (uint8_t []){0x00}, 1, 0},
    {0xAF, (uint8_t []){0x00}, 1, 0},
    {0xB0, (uint8_t []){0x48}, 1, 0},
    {0xB1, (uint8_t []){0x00}, 1, 0},
    {0xB2, (uint8_t []){0x09}, 1, 0},
    {0xB3, (uint8_t []){0x02}, 1, 0},
    {0xB4, (uint8_t []){0xD9}, 1, 0},
    {0xB5, (uint8_t []){0x04}, 1, 0},
    {0xB6, (uint8_t []){0x00}, 1, 0},
    {0xB7, (uint8_t []){0x00}, 1, 0},
    {0xB8, (uint8_t []){0x48}, 1, 0},
    {0xB9, (uint8_t []){0x00}, 1, 0},
    {0xBA, (uint8_t []){0x0B}, 1, 0},
    {0xBB, (uint8_t []){0x02}, 1, 0},
    {0xBC, (uint8_t []){0xDB}, 1, 0},
    {0xBD, (uint8_t []){0x04}, 1, 0},
    {0xBE, (uint8_t []){0x00}, 1, 0},
    {0xBF, (uint8_t []){0x00}, 1, 0},
    {0xC0, (uint8_t []){0x10}, 1, 0},
    {0xC1, (uint8_t []){0x47}, 1, 0},
    {0xC2, (uint8_t []){0x56}, 1, 0},
    {0xC3, (uint8_t []){0x65}, 1, 0},
    {0xC4, (uint8_t []){0x74}, 1, 0},
    {0xC5, (uint8_t []){0x88}, 1, 0},
    {0xC6, (uint8_t []){0x99}, 1, 0},
    {0xC7, (uint8_t []){0x01}, 1, 0},
    {0xC8, (uint8_t []){0xBB}, 1, 0},
    {0xC9, (uint8_t []){0xAA}, 1, 0},
    {0xD0, (uint8_t []){0x10}, 1, 0},
    {0xD1, (uint8_t []){0x47}, 1, 0},
    {0xD2, (uint8_t []){0x56}, 1, 0},
    {0xD3, (uint8_t []){0x65}, 1, 0},
    {0xD4, (uint8_t []){0x74}, 1, 0},
    {0xD5, (uint8_t []){0x88}, 1, 0},
    {0xD6, (uint8_t []){0x99}, 1, 0},
    {0xD7, (uint8_t []){0x01}, 1, 0},
    {0xD8, (uint8_t []){0xBB}, 1, 0},
    {0xD9, (uint8_t []){0xAA}, 1, 0},
    {0xF3, (uint8_t []){0x01}, 1, 0},
    {0xF0, (uint8_t []){0x00}, 1, 0},
    {0x21, (uint8_t []){}, 0, 0},
    {0x11, (uint8_t []){}, 0, 0},
    {0x00, (uint8_t []){}, 0, 120},
};
#endif

_Static_assert(sizeof(vendor_specific_init_yysj) /
                   sizeof(vendor_specific_init_yysj[0]) ==
                   184,
               "locked ST77916 command count changed");
_Static_assert(PCB_V1_LCD_STRIP_BYTES == 57600,
               "DMA strip must remain exactly 57,600 bytes");

typedef struct {
    bool bus_initialized;
    esp_lcd_panel_io_handle_t io;
    esp_lcd_panel_handle_t panel;
    SemaphoreHandle_t transfer_done;
    uint8_t *strip;
    volatile bool transfer_in_flight;
} display_context_t;

static const pcb_v1_display_state_t s_state_sequence[] = {
    PCB_V1_DISPLAY_STATE_BOOT_MARKER,
    PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF,
    PCB_V1_DISPLAY_STATE_QSPI_BUS_INIT,
    PCB_V1_DISPLAY_STATE_PANEL_IO_CREATE,
    PCB_V1_DISPLAY_STATE_PANEL_RESET,
    PCB_V1_DISPLAY_STATE_PANEL_INIT,
    PCB_V1_DISPLAY_STATE_DISPLAY_ON,
    PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
    PCB_V1_DISPLAY_STATE_BACKLIGHT_POLICY_GATE,
    PCB_V1_DISPLAY_STATE_READY,
};

static bool s_has_run;
static display_context_t s_context;
static pcb_v1_display_result_t s_stored_result = {
    .terminal_state = PCB_V1_DISPLAY_STATE_FAIL_SAFE,
    .failed_state = PCB_V1_DISPLAY_STATE_BOOT_MARKER,
    .error = ESP_ERR_INVALID_STATE,
};

const char *pcb_v1_display_state_name(pcb_v1_display_state_t state)
{
    static const char *const names[] = {
        [PCB_V1_DISPLAY_STATE_BOOT_MARKER] = "BOOT_MARKER",
        [PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF] = "BACKLIGHT_FORCED_OFF",
        [PCB_V1_DISPLAY_STATE_QSPI_BUS_INIT] = "QSPI_BUS_INIT",
        [PCB_V1_DISPLAY_STATE_PANEL_IO_CREATE] = "PANEL_IO_CREATE",
        [PCB_V1_DISPLAY_STATE_PANEL_RESET] = "PANEL_RESET",
        [PCB_V1_DISPLAY_STATE_PANEL_INIT] = "PANEL_INIT",
        [PCB_V1_DISPLAY_STATE_DISPLAY_ON] = "DISPLAY_ON",
        [PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW] = "TEST_PATTERN_DRAW",
        [PCB_V1_DISPLAY_STATE_BACKLIGHT_POLICY_GATE] = "BACKLIGHT_POLICY_GATE",
        [PCB_V1_DISPLAY_STATE_READY] = "READY",
        [PCB_V1_DISPLAY_STATE_FAIL_SAFE] = "FAIL_SAFE",
    };

    if (state < PCB_V1_DISPLAY_STATE_BOOT_MARKER ||
        state > PCB_V1_DISPLAY_STATE_FAIL_SAFE) {
        return "UNKNOWN";
    }
    return names[state];
}

static esp_err_t force_backlight_off(void)
{
    HOST_FAULT_POINT(FP_GPIO_PRELOAD_LOW);
    esp_err_t error = gpio_set_level(LCD_GPIO_BACKLIGHT, 0);
    if (error != ESP_OK) {
        return error;
    }

    const gpio_config_t configuration = {
        .pin_bit_mask = 1ULL << LCD_GPIO_BACKLIGHT,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    HOST_FAULT_POINT(FP_GPIO_CONFIG_OUTPUT);
    error = gpio_config(&configuration);
    if (error != ESP_OK) {
        return error;
    }

    HOST_FAULT_POINT(FP_GPIO_REASSERT_LOW);
    error = gpio_set_level(LCD_GPIO_BACKLIGHT, 0);
    if (error != ESP_OK) {
        return error;
    }
    HOST_FAULT_POINT(FP_GPIO_READBACK_LOW);
    return gpio_get_level(LCD_GPIO_BACKLIGHT) == 0 ? ESP_OK : ESP_ERR_INVALID_STATE;
}

static void best_effort_hold_backlight_low(void)
{
    HOST_FAULT_POINT(FP_GPIO_HOLD_LOW);
    (void)gpio_set_level(LCD_GPIO_BACKLIGHT, 0);
}

static bool color_transfer_done(esp_lcd_panel_io_handle_t panel_io,
                                esp_lcd_panel_io_event_data_t *event_data,
                                void *user_context)
{
    (void)panel_io;
    (void)event_data;
    display_context_t *context = user_context;
    BaseType_t high_priority_task_woken = pdFALSE;
    context->transfer_in_flight = false;
    if (context->transfer_done != NULL) {
        HOST_FAULT_POINT(FP_CALLBACK_SEMAPHORE_GIVE);
        xSemaphoreGiveFromISR(context->transfer_done, &high_priority_task_woken);
    }
    return high_priority_task_woken == pdTRUE;
}

static esp_err_t initialize_qspi_bus(display_context_t *context)
{
    spi_bus_config_t bus_configuration = ST77916_PANEL_BUS_QSPI_CONFIG(
        LCD_GPIO_CLOCK,
        LCD_GPIO_DATA_0,
        LCD_GPIO_DATA_1,
        LCD_GPIO_DATA_2,
        LCD_GPIO_DATA_3,
        PCB_V1_LCD_STRIP_BYTES);

    HOST_FAULT_POINT(FP_SPI_BUS_INITIALIZE);
    const esp_err_t error =
        spi_bus_initialize(SPI2_HOST, &bus_configuration, SPI_DMA_CH_AUTO);
    if (error == ESP_OK) {
        context->bus_initialized = true;
    }
    return error;
}

static esp_err_t create_panel_io(display_context_t *context)
{
    HOST_FAULT_POINT(FP_SEMAPHORE_CREATE);
    context->transfer_done = xSemaphoreCreateBinary();
    if (context->transfer_done == NULL) {
        return ESP_ERR_NO_MEM;
    }

    esp_lcd_panel_io_spi_config_t io_configuration =
        ST77916_PANEL_IO_QSPI_CONFIG(LCD_GPIO_CHIP_SELECT,
                                    color_transfer_done,
                                    context);
    io_configuration.dc_gpio_num = GPIO_NUM_NC;
    io_configuration.pclk_hz = LCD_PIXEL_CLOCK_HZ;
    io_configuration.trans_queue_depth = LCD_TRANSFER_QUEUE_DEPTH;

    HOST_FAULT_POINT(FP_PANEL_IO_CREATE);
    return esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                                    &io_configuration,
                                    &context->io);
}

static esp_err_t create_and_reset_panel(display_context_t *context)
{
    st77916_vendor_config_t vendor_configuration = {
        .init_cmds = vendor_specific_init_yysj,
        .init_cmds_size =
            sizeof(vendor_specific_init_yysj) / sizeof(vendor_specific_init_yysj[0]),
        .flags = {
            .use_qspi_interface = 1,
        },
    };
    const esp_lcd_panel_dev_config_t panel_configuration = {
        .reset_gpio_num = LCD_GPIO_RESET,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_configuration,
        .flags = {
            .reset_active_high = 0,
        },
    };

    HOST_FAULT_POINT(FP_PANEL_CREATE);
    esp_err_t error =
        esp_lcd_new_panel_st77916(context->io, &panel_configuration, &context->panel);
    if (error != ESP_OK) {
        return error;
    }
    HOST_FAULT_POINT(FP_PANEL_RESET);
    return esp_lcd_panel_reset(context->panel);
}

static esp_err_t initialize_panel(display_context_t *context)
{
    HOST_FAULT_POINT(FP_PANEL_INIT);
    esp_err_t error = esp_lcd_panel_init(context->panel);
    if (error != ESP_OK) {
        return error;
    }
    HOST_FAULT_POINT(FP_PANEL_MIRROR);
    error = esp_lcd_panel_mirror(context->panel, LCD_MIRROR_X, LCD_MIRROR_Y);
    if (error != ESP_OK) {
        return error;
    }
    HOST_FAULT_POINT(FP_PANEL_SWAP_XY);
    error = esp_lcd_panel_swap_xy(context->panel, LCD_SWAP_XY);
    if (error != ESP_OK) {
        return error;
    }
    HOST_FAULT_POINT(FP_PANEL_SET_GAP);
    return esp_lcd_panel_set_gap(context->panel, 0, 0);
}

static esp_err_t draw_test_pattern(display_context_t *context)
{
    static const uint16_t strip_boundaries[] = {0, 80, 160, 240, 320, 360};

    HOST_FAULT_POINT(FP_HEAP_ALLOCATE);
    context->strip = heap_caps_malloc(PCB_V1_LCD_STRIP_BYTES,
                                      MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (context->strip == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (((uintptr_t)context->strip & 0x3U) != 0) {
        return ESP_ERR_INVALID_STATE;
    }

    for (size_t index = 0;
         index + 1 < sizeof(strip_boundaries) / sizeof(strip_boundaries[0]);
         ++index) {
        HOST_FAULT_POINT((pcb_v1_display_fault_point_t)
                         (FP_GPIO_DRAW_GUARD_1 + index));
        if (gpio_get_level(LCD_GPIO_BACKLIGHT) != 0) {
            return ESP_ERR_INVALID_STATE;
        }

        const uint16_t y_start = strip_boundaries[index];
        const uint16_t y_end = strip_boundaries[index + 1];
        const uint16_t height = y_end - y_start;
        pcb_v1_display_pattern_fill_strip(context->strip,
                                          PCB_V1_LCD_STRIP_BYTES,
                                          y_start,
                                          height);

        context->transfer_in_flight = true;
        HOST_FAULT_POINT((pcb_v1_display_fault_point_t)
                         (FP_DRAW_SUBMIT_1 + index));
        const esp_err_t error =
            esp_lcd_panel_draw_bitmap(context->panel,
                                      0,
                                      y_start,
                                      PCB_V1_LCD_WIDTH,
                                      y_end,
                                      context->strip);
        if (error != ESP_OK) {
            context->transfer_in_flight = false;
            return error;
        }

        HOST_FAULT_POINT((pcb_v1_display_fault_point_t)
                         (FP_SEMAPHORE_WAIT_1 + index));
        if (xSemaphoreTake(context->transfer_done,
                           pdMS_TO_TICKS(LCD_TRANSFER_TIMEOUT_MS)) != pdTRUE) {
            return ESP_ERR_TIMEOUT;
        }
        if (context->transfer_in_flight) {
            return ESP_ERR_INVALID_STATE;
        }
    }

    HOST_FAULT_POINT(FP_STRIP_FREE_SUCCESS);
    free(context->strip);
    context->strip = NULL;
    HOST_FAULT_POINT(FP_SEMAPHORE_DELETE_SUCCESS);
    vSemaphoreDelete(context->transfer_done);
    context->transfer_done = NULL;
    return ESP_OK;
}

static esp_err_t retain_first_error(esp_err_t current_error,
                                    esp_err_t candidate_error)
{
    if (current_error == ESP_OK && candidate_error != ESP_OK) {
        return candidate_error;
    }
    return current_error;
}

static esp_err_t cleanup_failed_run(display_context_t *context,
                                    esp_err_t primary_error)
{
    esp_err_t result = primary_error;
    best_effort_hold_backlight_low();
    if (context->transfer_in_flight) {
        best_effort_hold_backlight_low();
        return result;
    }

    if (context->strip != NULL) {
        HOST_FAULT_POINT(FP_STRIP_FREE_CLEANUP);
        free(context->strip);
        context->strip = NULL;
    }
    best_effort_hold_backlight_low();

    if (context->panel != NULL) {
        HOST_FAULT_POINT(FP_PANEL_DELETE);
        const esp_err_t cleanup_error = esp_lcd_panel_del(context->panel);
        context->panel = NULL;
        result = retain_first_error(result, cleanup_error);
    }
    best_effort_hold_backlight_low();

    if (context->io != NULL) {
        HOST_FAULT_POINT(FP_PANEL_IO_DELETE);
        const esp_err_t cleanup_error = esp_lcd_panel_io_del(context->io);
        context->io = NULL;
        result = retain_first_error(result, cleanup_error);
    }
    best_effort_hold_backlight_low();

    if (context->transfer_done != NULL) {
        HOST_FAULT_POINT(FP_SEMAPHORE_DELETE_CLEANUP);
        vSemaphoreDelete(context->transfer_done);
        context->transfer_done = NULL;
    }
    best_effort_hold_backlight_low();

    if (context->bus_initialized) {
        HOST_FAULT_POINT(FP_SPI_BUS_FREE);
        const esp_err_t cleanup_error = spi_bus_free(SPI2_HOST);
        context->bus_initialized = false;
        result = retain_first_error(result, cleanup_error);
    }
    best_effort_hold_backlight_low();
    return result;
}

static esp_err_t execute_state(pcb_v1_display_state_t state,
                               display_context_t *context)
{
    switch (state) {
    case PCB_V1_DISPLAY_STATE_BOOT_MARKER:
        return ESP_OK;
    case PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF:
        return force_backlight_off();
    case PCB_V1_DISPLAY_STATE_QSPI_BUS_INIT:
        return initialize_qspi_bus(context);
    case PCB_V1_DISPLAY_STATE_PANEL_IO_CREATE:
        return create_panel_io(context);
    case PCB_V1_DISPLAY_STATE_PANEL_RESET:
        return create_and_reset_panel(context);
    case PCB_V1_DISPLAY_STATE_PANEL_INIT:
        return initialize_panel(context);
    case PCB_V1_DISPLAY_STATE_DISPLAY_ON:
        HOST_FAULT_POINT(FP_PANEL_DISPLAY_ON);
        return esp_lcd_panel_disp_on_off(context->panel, true);
    case PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW:
        return draw_test_pattern(context);
    case PCB_V1_DISPLAY_STATE_BACKLIGHT_POLICY_GATE:
        HOST_FAULT_POINT(FP_GPIO_POLICY_GUARD);
        if (gpio_get_level(LCD_GPIO_BACKLIGHT) != 0) {
            return ESP_ERR_INVALID_STATE;
        }
        puts("DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED");
        return ESP_OK;
    case PCB_V1_DISPLAY_STATE_READY:
        HOST_FAULT_POINT(FP_GPIO_READY_GUARD);
        if (gpio_get_level(LCD_GPIO_BACKLIGHT) != 0) {
            return ESP_ERR_INVALID_STATE;
        }
        puts("LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED");
        return ESP_OK;
    case PCB_V1_DISPLAY_STATE_FAIL_SAFE:
    default:
        return ESP_ERR_INVALID_ARG;
    }
}

typedef esp_err_t (*state_executor_t)(pcb_v1_display_state_t state,
                                     display_context_t *context);

static pcb_v1_display_result_t make_result(pcb_v1_display_state_t terminal_state,
                                           pcb_v1_display_state_t failed_state,
                                           esp_err_t error)
{
    pcb_v1_display_result_t result = {
        .terminal_state = terminal_state,
        .failed_state = failed_state,
        .error = error,
    };
    return result;
}

static pcb_v1_display_result_t run_state_sequence(state_executor_t executor)
{
    memset(&s_context, 0, sizeof(s_context));
    display_context_t *context = &s_context;
    for (size_t index = 0;
         index < sizeof(s_state_sequence) / sizeof(s_state_sequence[0]);
         ++index) {
        const pcb_v1_display_state_t state = s_state_sequence[index];
        printf("LCD_SM_ENTER %s\n", pcb_v1_display_state_name(state));
        const esp_err_t error = executor(state, context);
        if (error != ESP_OK) {
            printf("LCD_SM_FAIL state=%s err=%s\n",
                   pcb_v1_display_state_name(state),
                   esp_err_to_name(error));
            best_effort_hold_backlight_low();
            const esp_err_t final_error =
                cleanup_failed_run(context, error);
            best_effort_hold_backlight_low();
            return make_result(PCB_V1_DISPLAY_STATE_FAIL_SAFE,
                               state,
                               final_error);
        }
    }

    return make_result(PCB_V1_DISPLAY_STATE_READY,
                       PCB_V1_DISPLAY_STATE_READY,
                       ESP_OK);
}

pcb_v1_display_result_t pcb_v1_display_run_once(void)
{
    if (s_has_run) {
        return s_stored_result;
    }

    s_stored_result = run_state_sequence(execute_state);
    s_has_run = true;
    return s_stored_result;
}
