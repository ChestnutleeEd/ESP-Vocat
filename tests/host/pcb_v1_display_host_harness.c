#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcb_v1_display_host_fakes.h"
#include "../../firmware/main/pcb_v1_display.h"

enum {
    RESOURCE_BUS = 1U << 0,
    RESOURCE_SEMAPHORE = 1U << 1,
    RESOURCE_IO = 1U << 2,
    RESOURCE_PANEL = 1U << 3,
    RESOURCE_STRIP = 1U << 4,
};

typedef struct {
    pcb_v1_display_fault_point_t point;
    char api[40];
} api_call_t;

typedef struct {
    int gpio_level;
    int gpio_high_writes;
    int gpio_set_calls;
    int gpio_config_calls;
    int gpio_get_calls;
    int bus_init_calls;
    int bus_free_calls;
    int io_create_calls;
    int io_delete_calls;
    int panel_create_calls;
    int panel_reset_calls;
    int panel_init_calls;
    int panel_delete_calls;
    int display_on_calls;
    int draw_calls;
    int callback_calls;
    int semaphore_create_calls;
    int semaphore_take_calls;
    int semaphore_give_calls;
    int semaphore_delete_calls;
    int strip_allocations;
    int strip_frees;
    int buffer_reuse_violations;
    bool suppress_callback;
    bool transfer_active;
    esp_err_t panel_delete_result;
    esp_err_t io_delete_result;
    esp_err_t bus_free_result;
    void *strip_pointer;
    esp_lcd_panel_io_color_trans_done_cb_t callback;
    void *callback_context;
    int window_y_start[5];
    int window_y_end[5];
    char events[256][40];
    size_t event_count;
    api_call_t api_calls[256];
    size_t api_call_count;
    char logs[128][128];
    size_t log_count;
    bool bus_owned;
    bool semaphore_owned;
    bool io_owned;
    bool panel_owned;
    bool strip_owned;
    SemaphoreHandle_t semaphore_pointer;
    spi_host_device_t last_bus_host;
    int last_dma_channel;
    spi_bus_config_t last_bus_configuration;
    esp_lcd_spi_bus_handle_t last_io_bus;
    esp_lcd_panel_io_spi_config_t last_io_configuration;
    esp_lcd_panel_io_handle_t last_panel_io;
    int last_panel_reset_gpio;
    int last_panel_bits_per_pixel;
    size_t last_heap_size;
    uint32_t last_heap_capabilities;
    TickType_t last_wait_timeout;
} fake_backend_t;

typedef struct {
    pcb_v1_display_fault_point_t configured;
    int configured_calls;
    int injected_calls;
    size_t trigger_api_index;
    unsigned int owned_at_injection;
} fault_control_t;

static fake_backend_t g_fake;
static fault_control_t g_fault;
static int g_assertions;
static pcb_v1_display_fault_point_t g_current_fault_point;
static int g_total_point_calls[FP_COUNT];

#define CHECK(condition)                                                      \
    do {                                                                      \
        ++g_assertions;                                                       \
        if (!(condition)) {                                                   \
            fprintf(stderr, "CHECK failed at %s:%d: %s\n",                  \
                    __FILE__,                                                 \
                    __LINE__,                                                 \
                    #condition);                                              \
            exit(1);                                                          \
        }                                                                     \
    } while (0)

static void record_event(const char *event)
{
    if (g_fake.event_count < sizeof(g_fake.events) / sizeof(g_fake.events[0])) {
        snprintf(g_fake.events[g_fake.event_count],
                 sizeof(g_fake.events[g_fake.event_count]),
                 "%s",
                 event);
        ++g_fake.event_count;
    }
}

static int event_index(const char *event)
{
    for (size_t index = 0; index < g_fake.event_count; ++index) {
        if (strcmp(g_fake.events[index], event) == 0) {
            return (int)index;
        }
    }
    return -1;
}

static void record_api_call(const char *api)
{
    CHECK(g_current_fault_point > FP_NONE);
    CHECK(g_current_fault_point < FP_COUNT);
    CHECK(g_fake.api_call_count <
          sizeof(g_fake.api_calls) / sizeof(g_fake.api_calls[0]));
    api_call_t *call = &g_fake.api_calls[g_fake.api_call_count++];
    call->point = g_current_fault_point;
    snprintf(call->api, sizeof(call->api), "%s", api);
    ++g_total_point_calls[g_current_fault_point];
}

static unsigned int owned_resources(void)
{
    unsigned int resources = 0;
    if (g_fake.bus_owned) {
        resources |= RESOURCE_BUS;
    }
    if (g_fake.semaphore_owned) {
        resources |= RESOURCE_SEMAPHORE;
    }
    if (g_fake.io_owned) {
        resources |= RESOURCE_IO;
    }
    if (g_fake.panel_owned) {
        resources |= RESOURCE_PANEL;
    }
    if (g_fake.strip_owned) {
        resources |= RESOURCE_STRIP;
    }
    return resources;
}

static bool should_inject(void)
{
    if (g_fault.configured != g_current_fault_point) {
        return false;
    }
    ++g_fault.configured_calls;
    if (g_fault.injected_calls != 0) {
        return false;
    }
    ++g_fault.injected_calls;
    g_fault.trigger_api_index = g_fake.api_call_count - 1;
    g_fault.owned_at_injection = owned_resources();
    return true;
}

static void configure_fault(pcb_v1_display_fault_point_t point)
{
    CHECK(point > FP_NONE);
    CHECK(point < FP_COUNT);
    memset(&g_fault, 0, sizeof(g_fault));
    g_fault.configured = point;
}

static void record_log(const char *message)
{
    CHECK(g_fake.log_count < sizeof(g_fake.logs) / sizeof(g_fake.logs[0]));
    snprintf(g_fake.logs[g_fake.log_count],
             sizeof(g_fake.logs[g_fake.log_count]),
             "%s",
             message);
    ++g_fake.log_count;
}

static int tracked_printf(const char *format, ...)
{
    char message[128];
    va_list arguments;
    va_start(arguments, format);
    const int result = vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    record_log(message);
    fputs(message, stdout);
    return result;
}

static int tracked_puts(const char *message)
{
    record_log(message);
    fputs(message, stdout);
    fputc('\n', stdout);
    return 0;
}

static int log_count(const char *message)
{
    int count = 0;
    for (size_t index = 0; index < g_fake.log_count; ++index) {
        if (strcmp(g_fake.logs[index], message) == 0) {
            ++count;
        }
    }
    return count;
}

static void reset_backend(void)
{
    if (g_fake.strip_pointer != NULL) {
        free(g_fake.strip_pointer);
    }
    if (g_fake.semaphore_pointer != NULL) {
        free(g_fake.semaphore_pointer);
    }
    memset(&g_fake, 0, sizeof(g_fake));
    memset(&g_fault, 0, sizeof(g_fault));
    g_current_fault_point = FP_NONE;
    g_fake.gpio_level = 1;
    g_fake.panel_delete_result = ESP_OK;
    g_fake.io_delete_result = ESP_OK;
    g_fake.bus_free_result = ESP_OK;
}

static void tracked_free(void *pointer)
{
    record_api_call("free");
    if (pointer == g_fake.strip_pointer) {
        CHECK(g_fake.gpio_level == 0);
        CHECK(g_fake.strip_owned);
        CHECK(!g_fake.transfer_active);
        ++g_fake.strip_frees;
        g_fake.strip_pointer = NULL;
        g_fake.strip_owned = false;
        record_event(g_current_fault_point == FP_STRIP_FREE_SUCCESS
                         ? "strip_free_success"
                         : "strip_free");
    }
    free(pointer);
}

#define free tracked_free
#define printf tracked_printf
#define puts tracked_puts
#include "../../firmware/main/pcb_v1_display_test_pattern.c"
#include "../../firmware/main/pcb_v1_display.c"
#undef free
#undef printf
#undef puts

struct fake_semaphore {
    bool available;
};

void pcb_v1_display_host_set_fault_point(pcb_v1_display_fault_point_t point)
{
    CHECK(point > FP_NONE);
    CHECK(point < FP_COUNT);
    g_current_fault_point = point;
}

const char *esp_err_to_name(esp_err_t error)
{
    switch (error) {
    case ESP_OK:
        return "ESP_OK";
    case ESP_FAIL:
        return "ESP_FAIL";
    case ESP_ERR_NO_MEM:
        return "ESP_ERR_NO_MEM";
    case ESP_ERR_INVALID_ARG:
        return "ESP_ERR_INVALID_ARG";
    case ESP_ERR_INVALID_STATE:
        return "ESP_ERR_INVALID_STATE";
    case ESP_ERR_TIMEOUT:
        return "ESP_ERR_TIMEOUT";
    default:
        return "ESP_ERR_UNKNOWN";
    }
}

esp_err_t gpio_set_level(gpio_num_t gpio, uint32_t level)
{
    CHECK(g_current_fault_point == FP_GPIO_PRELOAD_LOW ||
          g_current_fault_point == FP_GPIO_REASSERT_LOW ||
          g_current_fault_point == FP_GPIO_HOLD_LOW);
    record_api_call("gpio_set_level");
    CHECK(gpio == GPIO_NUM_44);
    CHECK(level == 0);
    ++g_fake.gpio_set_calls;
    if (level != 0) {
        ++g_fake.gpio_high_writes;
    }
    if (should_inject()) {
        record_event("gpio_set_low_failed");
        return ESP_FAIL;
    }
    g_fake.gpio_level = (int)level;
    record_event("gpio_set_low");
    return ESP_OK;
}

esp_err_t gpio_config(const gpio_config_t *configuration)
{
    CHECK(g_current_fault_point == FP_GPIO_CONFIG_OUTPUT);
    record_api_call("gpio_config");
    CHECK(configuration->pin_bit_mask == (1ULL << GPIO_NUM_44));
    CHECK(configuration->mode == GPIO_MODE_OUTPUT);
    ++g_fake.gpio_config_calls;
    if (should_inject()) {
        record_event("gpio_config_failed");
        return ESP_ERR_INVALID_ARG;
    }
    record_event("gpio_config_output");
    return ESP_OK;
}

int gpio_get_level(gpio_num_t gpio)
{
    CHECK(g_current_fault_point == FP_GPIO_READBACK_LOW ||
          (g_current_fault_point >= FP_GPIO_DRAW_GUARD_1 &&
           g_current_fault_point <= FP_GPIO_DRAW_GUARD_5) ||
          g_current_fault_point == FP_GPIO_POLICY_GUARD ||
          g_current_fault_point == FP_GPIO_READY_GUARD);
    record_api_call("gpio_get_level");
    CHECK(gpio == GPIO_NUM_44);
    ++g_fake.gpio_get_calls;
    if (should_inject()) {
        record_event("gpio_read_high");
        return 1;
    }
    return g_fake.gpio_level;
}

esp_err_t spi_bus_initialize(spi_host_device_t host,
                             const spi_bus_config_t *configuration,
                             int dma_channel)
{
    CHECK(g_current_fault_point == FP_SPI_BUS_INITIALIZE);
    record_api_call("spi_bus_initialize");
    CHECK(host == SPI2_HOST);
    CHECK(dma_channel == SPI_DMA_CH_AUTO);
    CHECK(configuration->sclk_io_num == GPIO_NUM_18);
    CHECK(configuration->data0_io_num == GPIO_NUM_46);
    CHECK(configuration->data1_io_num == GPIO_NUM_13);
    CHECK(configuration->data2_io_num == GPIO_NUM_11);
    CHECK(configuration->data3_io_num == GPIO_NUM_12);
    CHECK(configuration->max_transfer_sz == 57600);
    ++g_fake.bus_init_calls;
    g_fake.last_bus_host = host;
    g_fake.last_dma_channel = dma_channel;
    g_fake.last_bus_configuration = *configuration;
    if (should_inject()) {
        record_event("bus_init_failed");
        return ESP_FAIL;
    }
    g_fake.bus_owned = true;
    record_event("bus_init");
    return ESP_OK;
}

esp_err_t spi_bus_free(spi_host_device_t host)
{
    CHECK(g_current_fault_point == FP_SPI_BUS_FREE);
    record_api_call("spi_bus_free");
    CHECK(host == SPI2_HOST);
    CHECK(g_fake.gpio_level == 0);
    CHECK(g_fake.bus_owned);
    ++g_fake.bus_free_calls;
    record_event("spi_bus_free");
    if (should_inject()) {
        return ESP_ERR_TIMEOUT;
    }
    if (g_fake.bus_free_result == ESP_OK) {
        g_fake.bus_owned = false;
    }
    return g_fake.bus_free_result;
}

esp_err_t esp_lcd_new_panel_io_spi(
    esp_lcd_spi_bus_handle_t bus,
    const esp_lcd_panel_io_spi_config_t *configuration,
    esp_lcd_panel_io_handle_t *output_io)
{
    CHECK(g_current_fault_point == FP_PANEL_IO_CREATE);
    record_api_call("esp_lcd_new_panel_io_spi");
    CHECK((intptr_t)bus == SPI2_HOST);
    CHECK(configuration->cs_gpio_num == GPIO_NUM_14);
    CHECK(configuration->dc_gpio_num == GPIO_NUM_NC);
    CHECK(configuration->pclk_hz == 40000000);
    CHECK(configuration->trans_queue_depth == 1);
    ++g_fake.io_create_calls;
    g_fake.last_io_bus = bus;
    g_fake.last_io_configuration = *configuration;
    if (should_inject()) {
        record_event("io_create_failed");
        return ESP_ERR_NO_MEM;
    }
    g_fake.callback = configuration->on_color_trans_done;
    g_fake.callback_context = configuration->user_ctx;
    *output_io = (void *)0x1100;
    g_fake.io_owned = true;
    record_event("io_create");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io)
{
    CHECK(g_current_fault_point == FP_PANEL_IO_DELETE);
    record_api_call("esp_lcd_panel_io_del");
    CHECK(io == (void *)0x1100);
    CHECK(g_fake.gpio_level == 0);
    CHECK(g_fake.io_owned);
    ++g_fake.io_delete_calls;
    record_event("panel_io_del");
    if (should_inject()) {
        return ESP_ERR_INVALID_STATE;
    }
    if (g_fake.io_delete_result == ESP_OK) {
        g_fake.io_owned = false;
    }
    return g_fake.io_delete_result;
}

esp_err_t esp_lcd_new_panel_st77916(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_dev_config_t *configuration,
    esp_lcd_panel_handle_t *output_panel)
{
    CHECK(g_current_fault_point == FP_PANEL_CREATE);
    record_api_call("esp_lcd_new_panel_st77916");
    const st77916_vendor_config_t *vendor = configuration->vendor_config;
    CHECK(io == (void *)0x1100);
    CHECK(configuration->reset_gpio_num == GPIO_NUM_3);
    CHECK(configuration->rgb_ele_order == LCD_RGB_ELEMENT_ORDER_RGB);
    CHECK(configuration->bits_per_pixel == 16);
    CHECK(configuration->flags.reset_active_high == 0);
    CHECK(vendor->init_cmds == vendor_specific_init_yysj);
    CHECK(vendor->init_cmds_size == 184);
    CHECK(vendor->flags.use_qspi_interface == 1);
    ++g_fake.panel_create_calls;
    g_fake.last_panel_io = io;
    g_fake.last_panel_reset_gpio = configuration->reset_gpio_num;
    g_fake.last_panel_bits_per_pixel = configuration->bits_per_pixel;
    if (should_inject()) {
        record_event("panel_create_failed");
        return ESP_ERR_NO_MEM;
    }
    *output_panel = (void *)0x2200;
    g_fake.panel_owned = true;
    record_event("panel_create");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel)
{
    CHECK(g_current_fault_point == FP_PANEL_RESET);
    record_api_call("esp_lcd_panel_reset");
    CHECK(panel == (void *)0x2200);
    ++g_fake.panel_reset_calls;
    if (should_inject()) {
        record_event("panel_reset_failed");
        return ESP_FAIL;
    }
    record_event("panel_reset");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel)
{
    CHECK(g_current_fault_point == FP_PANEL_INIT);
    record_api_call("esp_lcd_panel_init");
    CHECK(panel == (void *)0x2200);
    ++g_fake.panel_init_calls;
    if (should_inject()) {
        record_event("panel_init_failed");
        return ESP_FAIL;
    }
    record_event("panel_init");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t panel,
                               bool mirror_x,
                               bool mirror_y)
{
    CHECK(g_current_fault_point == FP_PANEL_MIRROR);
    record_api_call("esp_lcd_panel_mirror");
    CHECK(panel == (void *)0x2200);
    CHECK(!mirror_x);
    CHECK(!mirror_y);
    if (should_inject()) {
        record_event("panel_mirror_failed");
        return ESP_FAIL;
    }
    record_event("panel_mirror_off");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t panel, bool swap_axes)
{
    CHECK(g_current_fault_point == FP_PANEL_SWAP_XY);
    record_api_call("esp_lcd_panel_swap_xy");
    CHECK(panel == (void *)0x2200);
    CHECK(!swap_axes);
    if (should_inject()) {
        record_event("panel_swap_failed");
        return ESP_FAIL;
    }
    record_event("panel_swap_off");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t panel,
                                int x_gap,
                                int y_gap)
{
    CHECK(g_current_fault_point == FP_PANEL_SET_GAP);
    record_api_call("esp_lcd_panel_set_gap");
    CHECK(panel == (void *)0x2200);
    CHECK(x_gap == 0);
    CHECK(y_gap == 0);
    if (should_inject()) {
        record_event("panel_gap_failed");
        return ESP_FAIL;
    }
    record_event("panel_gap_zero");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool enabled)
{
    CHECK(g_current_fault_point == FP_PANEL_DISPLAY_ON);
    record_api_call("esp_lcd_panel_disp_on_off");
    CHECK(panel == (void *)0x2200);
    CHECK(enabled);
    ++g_fake.display_on_calls;
    if (should_inject()) {
        record_event("display_on_failed");
        return ESP_FAIL;
    }
    record_event("display_on");
    return ESP_OK;
}

esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t panel,
                                    int x_start,
                                    int y_start,
                                    int x_end,
                                    int y_end,
                                    const void *color_data)
{
    CHECK(g_current_fault_point >= FP_DRAW_SUBMIT_1 &&
          g_current_fault_point <= FP_DRAW_SUBMIT_5);
    const pcb_v1_display_fault_point_t draw_point = g_current_fault_point;
    record_api_call("esp_lcd_panel_draw_bitmap");
    CHECK(panel == (void *)0x2200);
    CHECK(x_start == 0);
    CHECK(x_end == 360);
    CHECK(color_data == g_fake.strip_pointer);
    if (g_fake.transfer_active) {
        ++g_fake.buffer_reuse_violations;
    }
    g_fake.transfer_active = true;
    const int index = g_fake.draw_calls;
    CHECK(index < 5);
    g_fake.window_y_start[index] = y_start;
    g_fake.window_y_end[index] = y_end;
    ++g_fake.draw_calls;
    record_event("draw");
    const bool suppress_callback = should_inject() || g_fake.suppress_callback;
    if (!suppress_callback) {
        esp_lcd_panel_io_event_data_t event_data = {0};
        ++g_fake.callback_calls;
        g_fake.callback((void *)0x1100, &event_data, g_fake.callback_context);
        g_fake.transfer_active = false;
    }
    CHECK(draw_point >= FP_DRAW_SUBMIT_1 && draw_point <= FP_DRAW_SUBMIT_5);
    return ESP_OK;
}

esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel)
{
    CHECK(g_current_fault_point == FP_PANEL_DELETE);
    record_api_call("esp_lcd_panel_del");
    CHECK(panel == (void *)0x2200);
    CHECK(g_fake.gpio_level == 0);
    CHECK(g_fake.panel_owned);
    ++g_fake.panel_delete_calls;
    record_event("panel_del");
    if (should_inject()) {
        return ESP_FAIL;
    }
    if (g_fake.panel_delete_result == ESP_OK) {
        g_fake.panel_owned = false;
    }
    return g_fake.panel_delete_result;
}

void *heap_caps_malloc(size_t size, uint32_t capabilities)
{
    CHECK(g_current_fault_point == FP_HEAP_ALLOCATE);
    record_api_call("heap_caps_malloc");
    CHECK(size == 57600);
    CHECK(capabilities == (MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
    ++g_fake.strip_allocations;
    g_fake.last_heap_size = size;
    g_fake.last_heap_capabilities = capabilities;
    if (should_inject()) {
        record_event("strip_allocate_failed");
        return NULL;
    }
    g_fake.strip_pointer = malloc(size);
    CHECK(g_fake.strip_pointer != NULL);
    g_fake.strip_owned = true;
    record_event("strip_allocate");
    return g_fake.strip_pointer;
}

SemaphoreHandle_t xSemaphoreCreateBinary(void)
{
    CHECK(g_current_fault_point == FP_SEMAPHORE_CREATE);
    record_api_call("xSemaphoreCreateBinary");
    ++g_fake.semaphore_create_calls;
    if (should_inject()) {
        record_event("semaphore_create_failed");
        return NULL;
    }
    SemaphoreHandle_t semaphore = malloc(sizeof(*semaphore));
    CHECK(semaphore != NULL);
    semaphore->available = false;
    g_fake.semaphore_pointer = semaphore;
    g_fake.semaphore_owned = true;
    record_event("semaphore_create");
    return semaphore;
}

BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t semaphore,
                                 BaseType_t *high_priority_task_woken)
{
    CHECK(g_current_fault_point == FP_CALLBACK_SEMAPHORE_GIVE);
    record_api_call("xSemaphoreGiveFromISR");
    CHECK(semaphore == g_fake.semaphore_pointer);
    CHECK(g_fake.semaphore_owned);
    ++g_fake.semaphore_give_calls;
    semaphore->available = true;
    *high_priority_task_woken = pdFALSE;
    record_event("callback_done");
    return pdTRUE;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t timeout)
{
    CHECK(g_current_fault_point >= FP_SEMAPHORE_WAIT_1 &&
          g_current_fault_point <= FP_SEMAPHORE_WAIT_5);
    record_api_call("xSemaphoreTake");
    CHECK(semaphore == g_fake.semaphore_pointer);
    CHECK(g_fake.semaphore_owned);
    CHECK(timeout == 1000);
    ++g_fake.semaphore_take_calls;
    g_fake.last_wait_timeout = timeout;
    if (should_inject()) {
        record_event("callback_wait_failed");
        return pdFALSE;
    }
    if (!semaphore->available) {
        return pdFALSE;
    }
    semaphore->available = false;
    record_event("callback_wait");
    return pdTRUE;
}

void vSemaphoreDelete(SemaphoreHandle_t semaphore)
{
    CHECK(g_current_fault_point == FP_SEMAPHORE_DELETE_SUCCESS ||
          g_current_fault_point == FP_SEMAPHORE_DELETE_CLEANUP);
    record_api_call("vSemaphoreDelete");
    CHECK(g_fake.gpio_level == 0);
    CHECK(semaphore == g_fake.semaphore_pointer);
    CHECK(g_fake.semaphore_owned);
    ++g_fake.semaphore_delete_calls;
    record_event(g_current_fault_point == FP_SEMAPHORE_DELETE_SUCCESS
                     ? "semaphore_delete_success"
                     : "semaphore_delete");
    g_fake.semaphore_pointer = NULL;
    g_fake.semaphore_owned = false;
    free(semaphore);
}

static uint16_t read_pixel(const uint8_t *buffer, uint16_t x, uint16_t row)
{
    const size_t offset = ((size_t)row * PCB_V1_LCD_WIDTH + x) * 2;
    return ((uint16_t)buffer[offset] << 8) | buffer[offset + 1];
}

static bool is_cleanup_resource_event(const char *event)
{
    return strcmp(event, "strip_free") == 0 ||
           strcmp(event, "panel_del") == 0 ||
           strcmp(event, "panel_io_del") == 0 ||
           strcmp(event, "semaphore_delete") == 0 ||
           strcmp(event, "spi_bus_free") == 0;
}

static void check_cleanup_event_sequence(const char *const *expected,
                                         size_t expected_count)
{
    size_t actual_index = 0;
    for (size_t index = 0; index < g_fake.event_count; ++index) {
        if (!is_cleanup_resource_event(g_fake.events[index])) {
            continue;
        }
        CHECK(actual_index < expected_count);
        CHECK(strcmp(g_fake.events[index], expected[actual_index]) == 0);
        ++actual_index;
    }
    CHECK(actual_index == expected_count);
}

static display_context_t make_cleanup_context(bool has_strip,
                                              bool has_panel,
                                              bool has_io,
                                              bool has_semaphore,
                                              bool has_bus)
{
    display_context_t context = {0};
    context.bus_initialized = has_bus;
    g_fake.bus_owned = has_bus;
    context.io = has_io ? (void *)0x1100 : NULL;
    g_fake.io_owned = has_io;
    context.panel = has_panel ? (void *)0x2200 : NULL;
    g_fake.panel_owned = has_panel;
    if (has_semaphore) {
        context.transfer_done = malloc(sizeof(*context.transfer_done));
        CHECK(context.transfer_done != NULL);
        context.transfer_done->available = false;
        g_fake.semaphore_pointer = context.transfer_done;
        g_fake.semaphore_owned = true;
    }
    if (has_strip) {
        g_fake.strip_pointer = malloc(16);
        CHECK(g_fake.strip_pointer != NULL);
        context.strip = g_fake.strip_pointer;
        g_fake.strip_owned = true;
    }
    g_fake.gpio_level = 0;
    g_fake.event_count = 0;
    g_fake.api_call_count = 0;
    return context;
}

static void check_cleanup_context_cleared(const display_context_t *context)
{
    CHECK(context->strip == NULL);
    CHECK(context->panel == NULL);
    CHECK(context->io == NULL);
    CHECK(context->transfer_done == NULL);
    CHECK(!context->bus_initialized);
    CHECK(!context->transfer_in_flight);
}

typedef struct {
    const char *id;
    const char *api;
    bool failable;
} fault_point_info_t;

static const fault_point_info_t s_fault_points[FP_COUNT] = {
    [FP_GPIO_PRELOAD_LOW] = {"FP_GPIO_PRELOAD_LOW", "gpio_set_level", true},
    [FP_GPIO_CONFIG_OUTPUT] = {"FP_GPIO_CONFIG_OUTPUT", "gpio_config", true},
    [FP_GPIO_REASSERT_LOW] = {"FP_GPIO_REASSERT_LOW", "gpio_set_level", true},
    [FP_GPIO_READBACK_LOW] = {"FP_GPIO_READBACK_LOW", "gpio_get_level", true},
    [FP_GPIO_HOLD_LOW] = {"FP_GPIO_HOLD_LOW", "gpio_set_level", true},
    [FP_CALLBACK_SEMAPHORE_GIVE] = {
        "FP_CALLBACK_SEMAPHORE_GIVE", "xSemaphoreGiveFromISR", false},
    [FP_SPI_BUS_INITIALIZE] = {
        "FP_SPI_BUS_INITIALIZE", "spi_bus_initialize", true},
    [FP_SEMAPHORE_CREATE] = {
        "FP_SEMAPHORE_CREATE", "xSemaphoreCreateBinary", true},
    [FP_PANEL_IO_CREATE] = {
        "FP_PANEL_IO_CREATE", "esp_lcd_new_panel_io_spi", true},
    [FP_PANEL_CREATE] = {
        "FP_PANEL_CREATE", "esp_lcd_new_panel_st77916", true},
    [FP_PANEL_RESET] = {"FP_PANEL_RESET", "esp_lcd_panel_reset", true},
    [FP_PANEL_INIT] = {"FP_PANEL_INIT", "esp_lcd_panel_init", true},
    [FP_PANEL_MIRROR] = {"FP_PANEL_MIRROR", "esp_lcd_panel_mirror", true},
    [FP_PANEL_SWAP_XY] = {
        "FP_PANEL_SWAP_XY", "esp_lcd_panel_swap_xy", true},
    [FP_PANEL_SET_GAP] = {"FP_PANEL_SET_GAP", "esp_lcd_panel_set_gap", true},
    [FP_HEAP_ALLOCATE] = {"FP_HEAP_ALLOCATE", "heap_caps_malloc", true},
    [FP_GPIO_DRAW_GUARD_1] = {
        "FP_GPIO_DRAW_GUARD_1", "gpio_get_level", true},
    [FP_GPIO_DRAW_GUARD_2] = {
        "FP_GPIO_DRAW_GUARD_2", "gpio_get_level", true},
    [FP_GPIO_DRAW_GUARD_3] = {
        "FP_GPIO_DRAW_GUARD_3", "gpio_get_level", true},
    [FP_GPIO_DRAW_GUARD_4] = {
        "FP_GPIO_DRAW_GUARD_4", "gpio_get_level", true},
    [FP_GPIO_DRAW_GUARD_5] = {
        "FP_GPIO_DRAW_GUARD_5", "gpio_get_level", true},
    [FP_DRAW_SUBMIT_1] = {
        "FP_DRAW_SUBMIT_1", "esp_lcd_panel_draw_bitmap", true},
    [FP_DRAW_SUBMIT_2] = {
        "FP_DRAW_SUBMIT_2", "esp_lcd_panel_draw_bitmap", true},
    [FP_DRAW_SUBMIT_3] = {
        "FP_DRAW_SUBMIT_3", "esp_lcd_panel_draw_bitmap", true},
    [FP_DRAW_SUBMIT_4] = {
        "FP_DRAW_SUBMIT_4", "esp_lcd_panel_draw_bitmap", true},
    [FP_DRAW_SUBMIT_5] = {
        "FP_DRAW_SUBMIT_5", "esp_lcd_panel_draw_bitmap", true},
    [FP_SEMAPHORE_WAIT_1] = {
        "FP_SEMAPHORE_WAIT_1", "xSemaphoreTake", true},
    [FP_SEMAPHORE_WAIT_2] = {
        "FP_SEMAPHORE_WAIT_2", "xSemaphoreTake", true},
    [FP_SEMAPHORE_WAIT_3] = {
        "FP_SEMAPHORE_WAIT_3", "xSemaphoreTake", true},
    [FP_SEMAPHORE_WAIT_4] = {
        "FP_SEMAPHORE_WAIT_4", "xSemaphoreTake", true},
    [FP_SEMAPHORE_WAIT_5] = {
        "FP_SEMAPHORE_WAIT_5", "xSemaphoreTake", true},
    [FP_STRIP_FREE_SUCCESS] = {
        "FP_STRIP_FREE_SUCCESS", "free", false},
    [FP_SEMAPHORE_DELETE_SUCCESS] = {
        "FP_SEMAPHORE_DELETE_SUCCESS", "vSemaphoreDelete", false},
    [FP_PANEL_DISPLAY_ON] = {
        "FP_PANEL_DISPLAY_ON", "esp_lcd_panel_disp_on_off", true},
    [FP_GPIO_POLICY_GUARD] = {
        "FP_GPIO_POLICY_GUARD", "gpio_get_level", true},
    [FP_GPIO_READY_GUARD] = {
        "FP_GPIO_READY_GUARD", "gpio_get_level", true},
    [FP_STRIP_FREE_CLEANUP] = {
        "FP_STRIP_FREE_CLEANUP", "free", false},
    [FP_PANEL_DELETE] = {"FP_PANEL_DELETE", "esp_lcd_panel_del", true},
    [FP_PANEL_IO_DELETE] = {
        "FP_PANEL_IO_DELETE", "esp_lcd_panel_io_del", true},
    [FP_SEMAPHORE_DELETE_CLEANUP] = {
        "FP_SEMAPHORE_DELETE_CLEANUP", "vSemaphoreDelete", false},
    [FP_SPI_BUS_FREE] = {"FP_SPI_BUS_FREE", "spi_bus_free", true},
};

static const pcb_v1_display_fault_point_t s_happy_api_sequence[] = {
    FP_GPIO_PRELOAD_LOW,
    FP_GPIO_CONFIG_OUTPUT,
    FP_GPIO_REASSERT_LOW,
    FP_GPIO_READBACK_LOW,
    FP_SPI_BUS_INITIALIZE,
    FP_SEMAPHORE_CREATE,
    FP_PANEL_IO_CREATE,
    FP_PANEL_CREATE,
    FP_PANEL_RESET,
    FP_PANEL_INIT,
    FP_PANEL_MIRROR,
    FP_PANEL_SWAP_XY,
    FP_PANEL_SET_GAP,
    FP_PANEL_DISPLAY_ON,
    FP_HEAP_ALLOCATE,
    FP_GPIO_DRAW_GUARD_1,
    FP_DRAW_SUBMIT_1,
    FP_CALLBACK_SEMAPHORE_GIVE,
    FP_SEMAPHORE_WAIT_1,
    FP_GPIO_DRAW_GUARD_2,
    FP_DRAW_SUBMIT_2,
    FP_CALLBACK_SEMAPHORE_GIVE,
    FP_SEMAPHORE_WAIT_2,
    FP_GPIO_DRAW_GUARD_3,
    FP_DRAW_SUBMIT_3,
    FP_CALLBACK_SEMAPHORE_GIVE,
    FP_SEMAPHORE_WAIT_3,
    FP_GPIO_DRAW_GUARD_4,
    FP_DRAW_SUBMIT_4,
    FP_CALLBACK_SEMAPHORE_GIVE,
    FP_SEMAPHORE_WAIT_4,
    FP_GPIO_DRAW_GUARD_5,
    FP_DRAW_SUBMIT_5,
    FP_CALLBACK_SEMAPHORE_GIVE,
    FP_SEMAPHORE_WAIT_5,
    FP_STRIP_FREE_SUCCESS,
    FP_SEMAPHORE_DELETE_SUCCESS,
    FP_GPIO_POLICY_GUARD,
    FP_GPIO_READY_GUARD,
};

typedef enum {
    CLEANUP_NONE,
    CLEANUP_BUS,
    CLEANUP_SEMAPHORE_BUS,
    CLEANUP_IO_SEMAPHORE_BUS,
    CLEANUP_PANEL_IO_SEMAPHORE_BUS,
    CLEANUP_FULL,
    CLEANUP_PANEL_IO_BUS,
    CLEANUP_RETAIN_IN_FLIGHT,
} cleanup_kind_t;

typedef struct {
    pcb_v1_display_fault_point_t point;
    pcb_v1_display_state_t failed_state;
    esp_err_t expected_error;
    unsigned int owned_at_failure;
    cleanup_kind_t cleanup;
    int expected_draws;
    int expected_callbacks;
} state_fault_case_t;

static const state_fault_case_t s_state_fault_cases[] = {
    {FP_GPIO_PRELOAD_LOW, PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF,
     ESP_FAIL, 0, CLEANUP_NONE, 0, 0},
    {FP_GPIO_CONFIG_OUTPUT, PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF,
     ESP_ERR_INVALID_ARG, 0, CLEANUP_NONE, 0, 0},
    {FP_GPIO_REASSERT_LOW, PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF,
     ESP_FAIL, 0, CLEANUP_NONE, 0, 0},
    {FP_GPIO_READBACK_LOW, PCB_V1_DISPLAY_STATE_BACKLIGHT_FORCED_OFF,
     ESP_ERR_INVALID_STATE, 0, CLEANUP_NONE, 0, 0},
    {FP_SPI_BUS_INITIALIZE, PCB_V1_DISPLAY_STATE_QSPI_BUS_INIT,
     ESP_FAIL, 0, CLEANUP_NONE, 0, 0},
    {FP_SEMAPHORE_CREATE, PCB_V1_DISPLAY_STATE_PANEL_IO_CREATE,
     ESP_ERR_NO_MEM, RESOURCE_BUS, CLEANUP_BUS, 0, 0},
    {FP_PANEL_IO_CREATE, PCB_V1_DISPLAY_STATE_PANEL_IO_CREATE,
     ESP_ERR_NO_MEM, RESOURCE_BUS | RESOURCE_SEMAPHORE,
     CLEANUP_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_CREATE, PCB_V1_DISPLAY_STATE_PANEL_RESET,
     ESP_ERR_NO_MEM, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO,
     CLEANUP_IO_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_RESET, PCB_V1_DISPLAY_STATE_PANEL_RESET,
     ESP_FAIL, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_INIT, PCB_V1_DISPLAY_STATE_PANEL_INIT,
     ESP_FAIL, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_MIRROR, PCB_V1_DISPLAY_STATE_PANEL_INIT,
     ESP_FAIL, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_SWAP_XY, PCB_V1_DISPLAY_STATE_PANEL_INIT,
     ESP_FAIL, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_SET_GAP, PCB_V1_DISPLAY_STATE_PANEL_INIT,
     ESP_FAIL, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_PANEL_DISPLAY_ON, PCB_V1_DISPLAY_STATE_DISPLAY_ON,
     ESP_FAIL, RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_HEAP_ALLOCATE, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_NO_MEM,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_SEMAPHORE_BUS, 0, 0},
    {FP_GPIO_DRAW_GUARD_1, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_INVALID_STATE,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 0, 0},
    {FP_GPIO_DRAW_GUARD_2, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_INVALID_STATE,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 1, 1},
    {FP_GPIO_DRAW_GUARD_3, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_INVALID_STATE,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 2, 2},
    {FP_GPIO_DRAW_GUARD_4, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_INVALID_STATE,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 3, 3},
    {FP_GPIO_DRAW_GUARD_5, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_INVALID_STATE,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 4, 4},
    {FP_DRAW_SUBMIT_1, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_RETAIN_IN_FLIGHT, 1, 0},
    {FP_DRAW_SUBMIT_2, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_RETAIN_IN_FLIGHT, 2, 1},
    {FP_DRAW_SUBMIT_3, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_RETAIN_IN_FLIGHT, 3, 2},
    {FP_DRAW_SUBMIT_4, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_RETAIN_IN_FLIGHT, 4, 3},
    {FP_DRAW_SUBMIT_5, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_RETAIN_IN_FLIGHT, 5, 4},
    {FP_SEMAPHORE_WAIT_1, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 1, 1},
    {FP_SEMAPHORE_WAIT_2, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 2, 2},
    {FP_SEMAPHORE_WAIT_3, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 3, 3},
    {FP_SEMAPHORE_WAIT_4, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 4, 4},
    {FP_SEMAPHORE_WAIT_5, PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW,
     ESP_ERR_TIMEOUT,
     RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
         RESOURCE_STRIP,
     CLEANUP_FULL, 5, 5},
    {FP_GPIO_POLICY_GUARD, PCB_V1_DISPLAY_STATE_BACKLIGHT_POLICY_GATE,
     ESP_ERR_INVALID_STATE, RESOURCE_BUS | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_BUS, 5, 5},
    {FP_GPIO_READY_GUARD, PCB_V1_DISPLAY_STATE_READY,
     ESP_ERR_INVALID_STATE, RESOURCE_BUS | RESOURCE_IO | RESOURCE_PANEL,
     CLEANUP_PANEL_IO_BUS, 5, 5},
};

static void check_cleanup_kind(cleanup_kind_t cleanup)
{
    static const char *const bus[] = {"spi_bus_free"};
    static const char *const semaphore_bus[] = {
        "semaphore_delete", "spi_bus_free"};
    static const char *const io_semaphore_bus[] = {
        "panel_io_del", "semaphore_delete", "spi_bus_free"};
    static const char *const panel_io_semaphore_bus[] = {
        "panel_del", "panel_io_del", "semaphore_delete", "spi_bus_free"};
    static const char *const full[] = {
        "strip_free", "panel_del", "panel_io_del",
        "semaphore_delete", "spi_bus_free"};
    static const char *const panel_io_bus[] = {
        "panel_del", "panel_io_del", "spi_bus_free"};

    switch (cleanup) {
    case CLEANUP_NONE:
    case CLEANUP_RETAIN_IN_FLIGHT:
        check_cleanup_event_sequence(NULL, 0);
        break;
    case CLEANUP_BUS:
        check_cleanup_event_sequence(bus, 1);
        break;
    case CLEANUP_SEMAPHORE_BUS:
        check_cleanup_event_sequence(semaphore_bus, 2);
        break;
    case CLEANUP_IO_SEMAPHORE_BUS:
        check_cleanup_event_sequence(io_semaphore_bus, 3);
        break;
    case CLEANUP_PANEL_IO_SEMAPHORE_BUS:
        check_cleanup_event_sequence(panel_io_semaphore_bus, 4);
        break;
    case CLEANUP_FULL:
        check_cleanup_event_sequence(full, 5);
        break;
    case CLEANUP_PANEL_IO_BUS:
        check_cleanup_event_sequence(panel_io_bus, 3);
        break;
    }
}

static void check_happy_prefix_through_fault(
    pcb_v1_display_fault_point_t target)
{
    size_t expected_index = 0;
    while (expected_index <
               sizeof(s_happy_api_sequence) / sizeof(s_happy_api_sequence[0]) &&
           s_happy_api_sequence[expected_index] != target) {
        ++expected_index;
    }
    CHECK(expected_index <
          sizeof(s_happy_api_sequence) / sizeof(s_happy_api_sequence[0]));
    CHECK(g_fault.trigger_api_index == expected_index);
    for (size_t index = 0; index <= expected_index; ++index) {
        CHECK(g_fake.api_calls[index].point == s_happy_api_sequence[index]);
    }
}

static void check_failure_markers(const state_fault_case_t *test_case)
{
    char failure[128];
    snprintf(failure,
             sizeof(failure),
             "LCD_SM_FAIL state=%s err=%s\n",
             pcb_v1_display_state_name(test_case->failed_state),
             esp_err_to_name(test_case->expected_error));
    CHECK(log_count(failure) == 1);
    CHECK(log_count(
              "LCD_SM_READY visual=UNVERIFIED "
              "backlight=DISABLED_NOT_AUTHORIZED") == 0);

    bool reached_failure_state = false;
    for (size_t index = 0;
         index < sizeof(s_state_sequence) / sizeof(s_state_sequence[0]);
         ++index) {
        char entry[128];
        snprintf(entry,
                 sizeof(entry),
                 "LCD_SM_ENTER %s\n",
                 pcb_v1_display_state_name(s_state_sequence[index]));
        if (!reached_failure_state) {
            CHECK(log_count(entry) == 1);
        } else {
            CHECK(log_count(entry) == 0);
        }
        if (s_state_sequence[index] == test_case->failed_state) {
            reached_failure_state = true;
        }
    }
    CHECK(reached_failure_state);
    CHECK(log_count("DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED") ==
          (test_case->failed_state == PCB_V1_DISPLAY_STATE_READY ? 1 : 0));
}

static void run_cleanup_success_case(bool has_strip,
                                     bool has_panel,
                                     bool has_io,
                                     bool has_semaphore,
                                     bool has_bus,
                                     const char *const *expected,
                                     size_t expected_count)
{
    reset_backend();
    display_context_t context = make_cleanup_context(has_strip,
                                                     has_panel,
                                                     has_io,
                                                     has_semaphore,
                                                     has_bus);
    const esp_err_t result = cleanup_failed_run(&context, ESP_OK);
    CHECK(result == ESP_OK);
    check_cleanup_event_sequence(expected, expected_count);
    CHECK(g_fake.strip_frees == (has_strip ? 1 : 0));
    CHECK(g_fake.panel_delete_calls == (has_panel ? 1 : 0));
    CHECK(g_fake.io_delete_calls == (has_io ? 1 : 0));
    CHECK(g_fake.semaphore_delete_calls == (has_semaphore ? 1 : 0));
    CHECK(g_fake.bus_free_calls == (has_bus ? 1 : 0));
    CHECK(g_fake.gpio_level == 0);
    check_cleanup_context_cleared(&context);

    const int strip_frees = g_fake.strip_frees;
    const int panel_delete_calls = g_fake.panel_delete_calls;
    const int io_delete_calls = g_fake.io_delete_calls;
    const int semaphore_delete_calls = g_fake.semaphore_delete_calls;
    const int bus_free_calls = g_fake.bus_free_calls;
    CHECK(cleanup_failed_run(&context, ESP_OK) == ESP_OK);
    CHECK(g_fake.strip_frees == strip_frees);
    CHECK(g_fake.panel_delete_calls == panel_delete_calls);
    CHECK(g_fake.io_delete_calls == io_delete_calls);
    CHECK(g_fake.semaphore_delete_calls == semaphore_delete_calls);
    CHECK(g_fake.bus_free_calls == bus_free_calls);
}

static void test_pattern_pixels_and_bounds(void)
{
    uint8_t *buffer = malloc(PCB_V1_LCD_STRIP_BYTES + 2);
    CHECK(buffer != NULL);
    memset(buffer, 0xA5, PCB_V1_LCD_STRIP_BYTES + 2);
    pcb_v1_display_pattern_fill_strip(buffer,
                                      PCB_V1_LCD_STRIP_BYTES,
                                      0,
                                      80);
    CHECK(read_pixel(buffer, 0, 0) == RGB565_WHITE);
    CHECK(read_pixel(buffer, 359, 79) == RGB565_WHITE);
    CHECK(read_pixel(buffer, 10, 10) == RGB565_BLACK);
    CHECK(read_pixel(buffer, TEXT_ORIGIN_X, TEXT_ORIGIN_Y) == RGB565_WHITE);
    CHECK(buffer[PCB_V1_LCD_STRIP_BYTES] == 0xA5);
    CHECK(buffer[PCB_V1_LCD_STRIP_BYTES + 1] == 0xA5);

    pcb_v1_display_pattern_fill_strip(buffer,
                                      PCB_V1_LCD_STRIP_BYTES,
                                      80,
                                      80);
    CHECK(read_pixel(buffer, 10, 20) == RGB565_RED);
    CHECK(buffer[((size_t)20 * PCB_V1_LCD_WIDTH + 10) * 2] == 0xF8);
    CHECK(buffer[((size_t)20 * PCB_V1_LCD_WIDTH + 10) * 2 + 1] == 0x00);

    pcb_v1_display_pattern_fill_strip(buffer,
                                      PCB_V1_LCD_STRIP_BYTES,
                                      160,
                                      80);
    CHECK(read_pixel(buffer, 10, 30) == RGB565_GREEN);
    pcb_v1_display_pattern_fill_strip(buffer,
                                      PCB_V1_LCD_STRIP_BYTES,
                                      280,
                                      80);
    CHECK(read_pixel(buffer, 10, 10) == RGB565_BLUE);
    CHECK(read_pixel(buffer, 10, 79) == RGB565_WHITE);

    memset(buffer, 0x5A, PCB_V1_LCD_STRIP_BYTES);
    pcb_v1_display_pattern_fill_strip(buffer, 8, 0, 80);
    CHECK(buffer[0] == 0x5A);
    free(buffer);
}

static void test_success_path(void)
{
    reset_backend();
    const pcb_v1_display_result_t result = run_state_sequence(execute_state);
    CHECK(result.terminal_state == PCB_V1_DISPLAY_STATE_READY);
    CHECK(result.error == ESP_OK);
    CHECK(g_fake.gpio_level == 0);
    CHECK(g_fake.bus_init_calls == 1);
    CHECK(g_fake.io_create_calls == 1);
    CHECK(g_fake.panel_create_calls == 1);
    CHECK(g_fake.panel_reset_calls == 1);
    CHECK(g_fake.panel_init_calls == 1);
    CHECK(g_fake.display_on_calls == 1);
    CHECK(g_fake.strip_allocations == 1);
    CHECK(g_fake.strip_frees == 1);
    CHECK(g_fake.draw_calls == 5);
    CHECK(g_fake.callback_calls == 5);
    CHECK(g_fake.buffer_reuse_violations == 0);
    CHECK(g_fake.panel_delete_calls == 0);
    CHECK(g_fake.io_delete_calls == 0);
    CHECK(g_fake.bus_free_calls == 0);
    CHECK(event_index("gpio_set_low") >= 0);
    CHECK(event_index("gpio_config_output") > event_index("gpio_set_low"));
    CHECK(event_index("bus_init") > event_index("gpio_config_output"));

    static const int expected_boundaries[] = {0, 80, 160, 240, 320, 360};
    for (int index = 0; index < 5; ++index) {
        CHECK(g_fake.window_y_start[index] == expected_boundaries[index]);
        CHECK(g_fake.window_y_end[index] == expected_boundaries[index + 1]);
    }
}

static void test_low_level_fault_matrix(void)
{
    for (size_t index = 0;
         index < sizeof(s_state_fault_cases) / sizeof(s_state_fault_cases[0]);
         ++index) {
        const state_fault_case_t *test_case = &s_state_fault_cases[index];
        reset_backend();
        configure_fault(test_case->point);
        const pcb_v1_display_result_t result = run_state_sequence(execute_state);

        CHECK(result.terminal_state == PCB_V1_DISPLAY_STATE_FAIL_SAFE);
        CHECK(result.failed_state == test_case->failed_state);
        CHECK(result.error == test_case->expected_error);
        CHECK(g_fault.configured_calls >= 1);
        CHECK(g_fault.injected_calls == 1);
        CHECK(g_fault.owned_at_injection == test_case->owned_at_failure);
        CHECK(strcmp(g_fake.api_calls[g_fault.trigger_api_index].api,
                     s_fault_points[test_case->point].api) == 0);
        check_happy_prefix_through_fault(test_case->point);
        check_failure_markers(test_case);
        check_cleanup_kind(test_case->cleanup);

        CHECK(g_fake.gpio_level == 0);
        CHECK(g_fake.gpio_high_writes == 0);
        CHECK(g_fake.draw_calls == test_case->expected_draws);
        CHECK(g_fake.callback_calls == test_case->expected_callbacks);
        CHECK(g_fake.buffer_reuse_violations == 0);
        CHECK(g_fake.strip_frees <= 1);
        CHECK(g_fake.panel_delete_calls <= 1);
        CHECK(g_fake.io_delete_calls <= 1);
        CHECK(g_fake.semaphore_delete_calls <= 1);
        CHECK(g_fake.bus_free_calls <= 1);

        if (test_case->cleanup == CLEANUP_RETAIN_IN_FLIGHT) {
            CHECK(owned_resources() ==
                  (RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO |
                   RESOURCE_PANEL | RESOURCE_STRIP));
            CHECK(s_context.transfer_in_flight);
            CHECK(g_fake.transfer_active);
        } else {
            check_cleanup_context_cleared(&s_context);
            CHECK(owned_resources() == 0);
            CHECK(!g_fake.transfer_active);
        }
    }
}

static void test_cleanup_success_and_partial_orders(void)
{
    static const char *const full[] = {
        "strip_free",
        "panel_del",
        "panel_io_del",
        "semaphore_delete",
        "spi_bus_free",
    };
    static const char *const no_strip[] = {
        "panel_del",
        "panel_io_del",
        "semaphore_delete",
        "spi_bus_free",
    };
    static const char *const no_panel[] = {
        "panel_io_del",
        "semaphore_delete",
        "spi_bus_free",
    };
    static const char *const no_io[] = {
        "semaphore_delete",
        "spi_bus_free",
    };
    static const char *const bus_only[] = {
        "spi_bus_free",
    };

    run_cleanup_success_case(true, true, true, true, true, full, 5);
    run_cleanup_success_case(false, true, true, true, true, no_strip, 4);
    run_cleanup_success_case(false, false, true, true, true, no_panel, 3);
    run_cleanup_success_case(false, false, false, true, true, no_io, 2);
    run_cleanup_success_case(false, false, false, false, true, bus_only, 1);
}

static void test_each_failable_cleanup_api_continues(void)
{
    static const char *const expected[] = {
        "strip_free",
        "panel_del",
        "panel_io_del",
        "semaphore_delete",
        "spi_bus_free",
    };
    static const struct {
        pcb_v1_display_fault_point_t point;
        esp_err_t expected_error;
        unsigned int owned_at_injection;
        unsigned int retained_after_failure;
    } cases[] = {
        {FP_GPIO_HOLD_LOW, ESP_OK,
         RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL |
             RESOURCE_STRIP,
         0},
        {FP_PANEL_DELETE, ESP_FAIL,
         RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO | RESOURCE_PANEL,
         RESOURCE_PANEL},
        {FP_PANEL_IO_DELETE, ESP_ERR_INVALID_STATE,
         RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO,
         RESOURCE_IO},
        {FP_SPI_BUS_FREE, ESP_ERR_TIMEOUT, RESOURCE_BUS, RESOURCE_BUS},
    };

    for (size_t failure = 0;
         failure < sizeof(cases) / sizeof(cases[0]);
         ++failure) {
        reset_backend();
        display_context_t context =
            make_cleanup_context(true, true, true, true, true);
        configure_fault(cases[failure].point);

        const esp_err_t result = cleanup_failed_run(&context, ESP_OK);
        CHECK(result == cases[failure].expected_error);
        CHECK(g_fault.configured_calls >= 1);
        CHECK(g_fault.injected_calls == 1);
        CHECK(g_fault.owned_at_injection == cases[failure].owned_at_injection);
        CHECK(strcmp(g_fake.api_calls[g_fault.trigger_api_index].api,
                     s_fault_points[cases[failure].point].api) == 0);
        check_cleanup_event_sequence(expected, 5);
        CHECK(g_fake.strip_frees == 1);
        CHECK(g_fake.panel_delete_calls == 1);
        CHECK(g_fake.io_delete_calls == 1);
        CHECK(g_fake.semaphore_delete_calls == 1);
        CHECK(g_fake.bus_free_calls == 1);
        CHECK(g_fake.gpio_level == 0);
        CHECK(g_fake.gpio_high_writes == 0);
        check_cleanup_context_cleared(&context);
        CHECK(owned_resources() == cases[failure].retained_after_failure);

        const int strip_frees = g_fake.strip_frees;
        const int panel_delete_calls = g_fake.panel_delete_calls;
        const int io_delete_calls = g_fake.io_delete_calls;
        const int semaphore_delete_calls = g_fake.semaphore_delete_calls;
        const int bus_free_calls = g_fake.bus_free_calls;
        CHECK(cleanup_failed_run(&context, ESP_OK) == ESP_OK);
        CHECK(g_fake.strip_frees == strip_frees);
        CHECK(g_fake.panel_delete_calls == panel_delete_calls);
        CHECK(g_fake.io_delete_calls == io_delete_calls);
        CHECK(g_fake.semaphore_delete_calls == semaphore_delete_calls);
        CHECK(g_fake.bus_free_calls == bus_free_calls);
    }
}

static void test_cleanup_error_priority(void)
{
    static const char *const expected[] = {
        "strip_free",
        "panel_del",
        "panel_io_del",
        "semaphore_delete",
        "spi_bus_free",
    };
    reset_backend();
    display_context_t context =
        make_cleanup_context(true, true, true, true, true);
    configure_fault(FP_PANEL_DELETE);
    CHECK(cleanup_failed_run(&context, ESP_ERR_TIMEOUT) == ESP_ERR_TIMEOUT);
    check_cleanup_event_sequence(expected, 5);
    check_cleanup_context_cleared(&context);
    CHECK(g_fault.injected_calls == 1);
    CHECK(g_fake.strip_frees == 1);
    CHECK(g_fake.panel_delete_calls == 1);
    CHECK(g_fake.io_delete_calls == 1);
    CHECK(g_fake.semaphore_delete_calls == 1);
    CHECK(g_fake.bus_free_calls == 1);
    CHECK(g_fake.gpio_level == 0);
}

static void test_timeout_retains_in_flight_resources(void)
{
    reset_backend();
    g_fake.suppress_callback = true;
    const pcb_v1_display_result_t result = run_state_sequence(execute_state);
    CHECK(result.terminal_state == PCB_V1_DISPLAY_STATE_FAIL_SAFE);
    CHECK(result.failed_state == PCB_V1_DISPLAY_STATE_TEST_PATTERN_DRAW);
    CHECK(result.error == ESP_ERR_TIMEOUT);
    CHECK(g_fake.draw_calls == 1);
    CHECK(g_fake.strip_frees == 0);
    CHECK(g_fake.panel_delete_calls == 0);
    CHECK(g_fake.io_delete_calls == 0);
    CHECK(g_fake.bus_free_calls == 0);
    CHECK(g_fake.semaphore_delete_calls == 0);
    CHECK(g_fake.gpio_level == 0);
}

static void test_late_and_duplicate_callbacks_are_safe(void)
{
    reset_backend();
    configure_fault(FP_DRAW_SUBMIT_1);
    const pcb_v1_display_result_t timeout_result =
        run_state_sequence(execute_state);
    CHECK(timeout_result.error == ESP_ERR_TIMEOUT);
    CHECK(s_context.transfer_in_flight);
    CHECK(g_fake.semaphore_owned);
    CHECK(g_fake.strip_owned);
    const int strip_frees = g_fake.strip_frees;
    const int semaphore_deletes = g_fake.semaphore_delete_calls;
    const int panel_deletes = g_fake.panel_delete_calls;
    const int io_deletes = g_fake.io_delete_calls;
    const int bus_frees = g_fake.bus_free_calls;

    esp_lcd_panel_io_event_data_t event_data = {0};
    ++g_fake.callback_calls;
    g_fake.callback((void *)0x1100, &event_data, g_fake.callback_context);
    g_fake.transfer_active = false;
    CHECK(!s_context.transfer_in_flight);
    ++g_fake.callback_calls;
    g_fake.callback((void *)0x1100, &event_data, g_fake.callback_context);
    CHECK(g_fake.semaphore_give_calls == 2);
    CHECK(g_fake.strip_frees == strip_frees);
    CHECK(g_fake.semaphore_delete_calls == semaphore_deletes);
    CHECK(g_fake.panel_delete_calls == panel_deletes);
    CHECK(g_fake.io_delete_calls == io_deletes);
    CHECK(g_fake.bus_free_calls == bus_frees);
    CHECK(owned_resources() ==
          (RESOURCE_BUS | RESOURCE_SEMAPHORE | RESOURCE_IO |
           RESOURCE_PANEL | RESOURCE_STRIP));
    CHECK(g_fake.gpio_level == 0);

    reset_backend();
    configure_fault(FP_SEMAPHORE_WAIT_1);
    const pcb_v1_display_result_t wait_result =
        run_state_sequence(execute_state);
    CHECK(wait_result.error == ESP_ERR_TIMEOUT);
    CHECK(!g_fake.semaphore_owned);
    const size_t api_calls_after_cleanup = g_fake.api_call_count;
    const int gives_after_cleanup = g_fake.semaphore_give_calls;
    g_fake.callback((void *)0x1100, &event_data, g_fake.callback_context);
    CHECK(g_fake.api_call_count == api_calls_after_cleanup);
    CHECK(g_fake.semaphore_give_calls == gives_after_cleanup);
    CHECK(g_fake.gpio_level == 0);
}

static void test_run_once_idempotence(void)
{
    reset_backend();
    s_has_run = false;
    const pcb_v1_display_result_t first = pcb_v1_display_run_once();
    const size_t event_count = g_fake.event_count;
    const pcb_v1_display_result_t second = pcb_v1_display_run_once();
    CHECK(first.terminal_state == PCB_V1_DISPLAY_STATE_READY);
    CHECK(second.terminal_state == first.terminal_state);
    CHECK(second.error == first.error);
    CHECK(g_fake.event_count == event_count);
}

static void test_failed_run_once_idempotence(void)
{
    reset_backend();
    configure_fault(FP_SPI_BUS_INITIALIZE);
    s_has_run = false;
    const pcb_v1_display_result_t first = pcb_v1_display_run_once();
    const size_t api_call_count = g_fake.api_call_count;
    const size_t log_count_before_second = g_fake.log_count;
    const pcb_v1_display_result_t second = pcb_v1_display_run_once();
    CHECK(first.terminal_state == PCB_V1_DISPLAY_STATE_FAIL_SAFE);
    CHECK(first.failed_state == PCB_V1_DISPLAY_STATE_QSPI_BUS_INIT);
    CHECK(first.error == ESP_FAIL);
    CHECK(second.terminal_state == first.terminal_state);
    CHECK(second.failed_state == first.failed_state);
    CHECK(second.error == first.error);
    CHECK(g_fake.api_call_count == api_call_count);
    CHECK(g_fake.log_count == log_count_before_second);
}

static void test_fault_point_inventory_and_coverage(void)
{
    bool covered[FP_COUNT] = {false};
    size_t failable_count = 0;
    for (int point = FP_NONE + 1; point < FP_COUNT; ++point) {
        CHECK(s_fault_points[point].id != NULL);
        CHECK(s_fault_points[point].api != NULL);
        CHECK(g_total_point_calls[point] > 0);
        if (s_fault_points[point].failable) {
            ++failable_count;
        }
    }

    for (size_t index = 0;
         index < sizeof(s_state_fault_cases) / sizeof(s_state_fault_cases[0]);
         ++index) {
        const pcb_v1_display_fault_point_t point =
            s_state_fault_cases[index].point;
        CHECK(!covered[point]);
        covered[point] = true;
    }
    covered[FP_GPIO_HOLD_LOW] = true;
    covered[FP_PANEL_DELETE] = true;
    covered[FP_PANEL_IO_DELETE] = true;
    covered[FP_SPI_BUS_FREE] = true;

    size_t covered_failable = 0;
    for (int point = FP_NONE + 1; point < FP_COUNT; ++point) {
        if (!s_fault_points[point].failable) {
            continue;
        }
        CHECK(covered[point]);
        ++covered_failable;
    }
    CHECK((size_t)(FP_COUNT - 1) == 41);
    CHECK(failable_count == 36);
    CHECK(covered_failable == failable_count);
    CHECK(sizeof(s_state_fault_cases) / sizeof(s_state_fault_cases[0]) == 32);
}

int main(void)
{
    test_pattern_pixels_and_bounds();
    test_success_path();
    test_low_level_fault_matrix();
    test_cleanup_success_and_partial_orders();
    test_each_failable_cleanup_api_continues();
    test_cleanup_error_priority();
    test_timeout_retains_in_flight_resources();
    test_late_and_duplicate_callbacks_are_safe();
    test_run_once_idempotence();
    test_failed_run_once_idempotence();
    test_fault_point_inventory_and_coverage();
    printf("FAULT_MATRIX_C_TEST PASS points=41 failable=36 "
           "cases=36 passed=36 uncovered=0\n");
    printf("CLEANUP_C_TEST PASS order_cases=5 fault_cases=4 priority_cases=1\n");
    printf("HOST_C_TEST PASS assertions=%d\n", g_assertions);
    return 0;
}
