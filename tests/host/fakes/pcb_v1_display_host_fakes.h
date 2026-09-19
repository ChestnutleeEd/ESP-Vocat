#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int32_t esp_err_t;

#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_ERR_NO_MEM 0x101
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_STATE 0x103
#define ESP_ERR_TIMEOUT 0x107

const char *esp_err_to_name(esp_err_t error);

typedef enum {
    FP_NONE = 0,
    FP_GPIO_PRELOAD_LOW,
    FP_GPIO_CONFIG_OUTPUT,
    FP_GPIO_REASSERT_LOW,
    FP_GPIO_READBACK_LOW,
    FP_GPIO_HOLD_LOW,
    FP_CALLBACK_SEMAPHORE_GIVE,
    FP_SPI_BUS_INITIALIZE,
    FP_SEMAPHORE_CREATE,
    FP_PANEL_IO_CREATE,
    FP_PANEL_CREATE,
    FP_PANEL_RESET,
    FP_PANEL_INIT,
    FP_PANEL_MIRROR,
    FP_PANEL_SWAP_XY,
    FP_PANEL_SET_GAP,
    FP_HEAP_ALLOCATE,
    FP_GPIO_DRAW_GUARD_1,
    FP_GPIO_DRAW_GUARD_2,
    FP_GPIO_DRAW_GUARD_3,
    FP_GPIO_DRAW_GUARD_4,
    FP_GPIO_DRAW_GUARD_5,
    FP_DRAW_SUBMIT_1,
    FP_DRAW_SUBMIT_2,
    FP_DRAW_SUBMIT_3,
    FP_DRAW_SUBMIT_4,
    FP_DRAW_SUBMIT_5,
    FP_SEMAPHORE_WAIT_1,
    FP_SEMAPHORE_WAIT_2,
    FP_SEMAPHORE_WAIT_3,
    FP_SEMAPHORE_WAIT_4,
    FP_SEMAPHORE_WAIT_5,
    FP_STRIP_FREE_SUCCESS,
    FP_SEMAPHORE_DELETE_SUCCESS,
    FP_PANEL_DISPLAY_ON,
    FP_LEDC_TIMER_CONFIG,
    FP_LEDC_CHANNEL_CONFIG,
    FP_LEDC_ZERO_UPDATE,
    FP_LEDC_ZERO_VERIFY,
    FP_LEDC_ENABLE_UPDATE,
    FP_LEDC_ENABLE_VERIFY,
    FP_LEDC_CLEANUP_ZERO_UPDATE,
    FP_LEDC_STOP,
    FP_GPIO_CLEANUP_PRELOAD_LOW,
    FP_GPIO_CLEANUP_CONFIG_OUTPUT,
    FP_GPIO_CLEANUP_REASSERT_LOW,
    FP_GPIO_CLEANUP_READBACK_LOW,
    FP_STRIP_FREE_CLEANUP,
    FP_PANEL_DELETE,
    FP_PANEL_IO_DELETE,
    FP_SEMAPHORE_DELETE_CLEANUP,
    FP_SPI_BUS_FREE,
    FP_COUNT,
} pcb_v1_display_fault_point_t;

void pcb_v1_display_host_set_fault_point(pcb_v1_display_fault_point_t point);

typedef int gpio_num_t;
enum {
    GPIO_NUM_3 = 3,
    GPIO_NUM_11 = 11,
    GPIO_NUM_12 = 12,
    GPIO_NUM_13 = 13,
    GPIO_NUM_14 = 14,
    GPIO_NUM_18 = 18,
    GPIO_NUM_44 = 44,
    GPIO_NUM_46 = 46,
    GPIO_NUM_NC = -1,
};

enum {
    GPIO_MODE_OUTPUT = 1,
    GPIO_PULLUP_DISABLE = 0,
    GPIO_PULLDOWN_ENABLE = 1,
    GPIO_INTR_DISABLE = 0,
};

typedef struct {
    uint64_t pin_bit_mask;
    int mode;
    int pull_up_en;
    int pull_down_en;
    int intr_type;
} gpio_config_t;

esp_err_t gpio_set_level(gpio_num_t gpio, uint32_t level);
esp_err_t gpio_config(const gpio_config_t *configuration);
int gpio_get_level(gpio_num_t gpio);

typedef int ledc_mode_t;
typedef int ledc_timer_t;
typedef int ledc_channel_t;
typedef int ledc_intr_type_t;
typedef int ledc_timer_bit_t;
typedef int ledc_clk_cfg_t;
typedef int ledc_sleep_mode_t;

enum {
    LEDC_LOW_SPEED_MODE = 0,
    LEDC_TIMER_0 = 0,
    LEDC_CHANNEL_0 = 0,
    LEDC_INTR_DISABLE = 0,
    LEDC_TIMER_10_BIT = 10,
    LEDC_USE_APB_CLK = 1,
    LEDC_SLEEP_MODE_NO_ALIVE_NO_PD = 0,
};

typedef struct {
    int gpio_num;
    ledc_mode_t speed_mode;
    ledc_channel_t channel;
    ledc_intr_type_t intr_type;
    ledc_timer_t timer_sel;
    uint32_t duty;
    int hpoint;
    ledc_sleep_mode_t sleep_mode;
    struct {
        unsigned int output_invert : 1;
    } flags;
} ledc_channel_config_t;

typedef struct {
    ledc_mode_t speed_mode;
    ledc_timer_bit_t duty_resolution;
    ledc_timer_t timer_num;
    uint32_t freq_hz;
    ledc_clk_cfg_t clk_cfg;
    bool deconfigure;
} ledc_timer_config_t;

#define LEDC_ERR_DUTY UINT32_MAX

esp_err_t ledc_timer_config(const ledc_timer_config_t *configuration);
esp_err_t ledc_channel_config(const ledc_channel_config_t *configuration);
esp_err_t ledc_set_duty_and_update(ledc_mode_t speed_mode,
                                   ledc_channel_t channel,
                                   uint32_t duty,
                                   uint32_t hpoint);
uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
esp_err_t ledc_stop(ledc_mode_t speed_mode,
                    ledc_channel_t channel,
                    uint32_t idle_level);

typedef struct {
    int sclk_io_num;
    int data0_io_num;
    int data1_io_num;
    int data2_io_num;
    int data3_io_num;
    int max_transfer_sz;
} spi_bus_config_t;

typedef int spi_host_device_t;
enum {
    SPI2_HOST = 2,
    SPI_DMA_CH_AUTO = 3,
};

esp_err_t spi_bus_initialize(spi_host_device_t host,
                             const spi_bus_config_t *configuration,
                             int dma_channel);
esp_err_t spi_bus_free(spi_host_device_t host);

typedef void *esp_lcd_panel_io_handle_t;
typedef void *esp_lcd_panel_handle_t;
typedef void *esp_lcd_spi_bus_handle_t;

typedef struct {
    int unused;
} esp_lcd_panel_io_event_data_t;

typedef bool (*esp_lcd_panel_io_color_trans_done_cb_t)(
    esp_lcd_panel_io_handle_t panel_io,
    esp_lcd_panel_io_event_data_t *event_data,
    void *user_context);

typedef struct {
    int cs_gpio_num;
    int dc_gpio_num;
    unsigned int pclk_hz;
    size_t trans_queue_depth;
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done;
    void *user_ctx;
} esp_lcd_panel_io_spi_config_t;

typedef struct {
    int reset_gpio_num;
    int rgb_ele_order;
    int bits_per_pixel;
    const void *vendor_config;
    struct {
        unsigned int reset_active_high : 1;
    } flags;
} esp_lcd_panel_dev_config_t;

enum {
    LCD_RGB_ELEMENT_ORDER_RGB = 0,
};

typedef struct {
    int cmd;
    const void *data;
    size_t data_bytes;
    unsigned int delay_ms;
} st77916_lcd_init_cmd_t;

typedef struct {
    const st77916_lcd_init_cmd_t *init_cmds;
    uint16_t init_cmds_size;
    struct {
        unsigned int use_qspi_interface : 1;
    } flags;
} st77916_vendor_config_t;

static inline spi_bus_config_t fake_st77916_bus_configuration(
    int sclk,
    int d0,
    int d1,
    int d2,
    int d3,
    int maximum)
{
    spi_bus_config_t configuration = {
        .sclk_io_num = sclk,
        .data0_io_num = d0,
        .data1_io_num = d1,
        .data2_io_num = d2,
        .data3_io_num = d3,
        .max_transfer_sz = maximum,
    };
    return configuration;
}

static inline esp_lcd_panel_io_spi_config_t fake_st77916_io_configuration(
    int cs,
    esp_lcd_panel_io_color_trans_done_cb_t callback,
    void *context)
{
    esp_lcd_panel_io_spi_config_t configuration = {
        .cs_gpio_num = cs,
        .dc_gpio_num = GPIO_NUM_NC,
        .pclk_hz = 40000000,
        .trans_queue_depth = 10,
        .on_color_trans_done = callback,
        .user_ctx = context,
    };
    return configuration;
}

#define ST77916_PANEL_BUS_QSPI_CONFIG(sclk, d0, d1, d2, d3, maximum) \
    fake_st77916_bus_configuration((sclk), (d0), (d1), (d2), (d3), (maximum))

#define ST77916_PANEL_IO_QSPI_CONFIG(cs, callback, context) \
    fake_st77916_io_configuration((cs), (callback), (context))

esp_err_t esp_lcd_new_panel_io_spi(
    esp_lcd_spi_bus_handle_t bus,
    const esp_lcd_panel_io_spi_config_t *configuration,
    esp_lcd_panel_io_handle_t *output_io);
esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io);
esp_err_t esp_lcd_new_panel_st77916(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_dev_config_t *configuration,
    esp_lcd_panel_handle_t *output_panel);
esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t panel,
                               bool mirror_x,
                               bool mirror_y);
esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t panel, bool swap_axes);
esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t panel,
                                int x_gap,
                                int y_gap);
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool enabled);
esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t panel,
                                    int x_start,
                                    int y_start,
                                    int x_end,
                                    int y_end,
                                    const void *color_data);
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel);

#define MALLOC_CAP_DMA (1U << 0)
#define MALLOC_CAP_INTERNAL (1U << 1)
void *heap_caps_malloc(size_t size, uint32_t capabilities);

typedef int BaseType_t;
typedef uint32_t TickType_t;
typedef struct fake_semaphore *SemaphoreHandle_t;

#define pdFALSE 0
#define pdTRUE 1
#define pdMS_TO_TICKS(milliseconds) ((TickType_t)(milliseconds))

SemaphoreHandle_t xSemaphoreCreateBinary(void);
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t semaphore,
                                 BaseType_t *high_priority_task_woken);
BaseType_t xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t timeout);
void vSemaphoreDelete(SemaphoreHandle_t semaphore);
