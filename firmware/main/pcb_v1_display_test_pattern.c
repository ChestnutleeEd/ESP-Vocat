#include "pcb_v1_display_test_pattern.h"

#include <stdbool.h>
#include <string.h>

enum {
    RGB565_BLACK = 0x0000,
    RGB565_RED = 0xF800,
    RGB565_GREEN = 0x07E0,
    RGB565_BLUE = 0x001F,
    RGB565_WHITE = 0xFFFF,
    TEXT_GLYPH_WIDTH = 5,
    TEXT_GLYPH_HEIGHT = 7,
    TEXT_ADVANCE = 6,
    TEXT_ORIGIN_X = 126,
    TEXT_ORIGIN_Y = 41,
};

typedef struct {
    char character;
    uint8_t rows[TEXT_GLYPH_HEIGHT];
} glyph_t;

static const glyph_t s_glyphs[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {'-', {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}},
    {'C', {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}},
    {'D', {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E}},
    {'E', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}},
    {'L', {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}},
    {'P', {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}},
    {'S', {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}},
    {'T', {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {'V', {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}},
    {'a', {0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F}},
    {'o', {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E}},
    {'t', {0x04, 0x04, 0x1F, 0x04, 0x04, 0x05, 0x02}},
};

static const char s_test_text[] = "ESP-VoCat LCD TEST";

static uint16_t background_color(uint16_t y)
{
    if (y < 90) {
        return RGB565_BLACK;
    }
    if (y < 180) {
        return RGB565_RED;
    }
    if (y < 270) {
        return RGB565_GREEN;
    }
    return RGB565_BLUE;
}

static const glyph_t *find_glyph(char character)
{
    for (size_t index = 0; index < sizeof(s_glyphs) / sizeof(s_glyphs[0]); ++index) {
        if (s_glyphs[index].character == character) {
            return &s_glyphs[index];
        }
    }
    return &s_glyphs[0];
}

static bool text_pixel_is_set(uint16_t x, uint16_t y)
{
    if (x < TEXT_ORIGIN_X || y < TEXT_ORIGIN_Y ||
        y >= TEXT_ORIGIN_Y + TEXT_GLYPH_HEIGHT) {
        return false;
    }

    const uint16_t relative_x = x - TEXT_ORIGIN_X;
    const size_t character_index = relative_x / TEXT_ADVANCE;
    const uint16_t glyph_x = relative_x % TEXT_ADVANCE;
    if (character_index >= strlen(s_test_text) || glyph_x >= TEXT_GLYPH_WIDTH) {
        return false;
    }

    const glyph_t *glyph = find_glyph(s_test_text[character_index]);
    const uint8_t row = glyph->rows[y - TEXT_ORIGIN_Y];
    return (row & (1U << (TEXT_GLYPH_WIDTH - 1U - glyph_x))) != 0;
}

static uint16_t pattern_color(uint16_t x, uint16_t y)
{
    if (x == 0 || y == 0 || x == PCB_V1_LCD_WIDTH - 1 ||
        y == PCB_V1_LCD_HEIGHT - 1 || text_pixel_is_set(x, y)) {
        return RGB565_WHITE;
    }
    return background_color(y);
}

void pcb_v1_display_pattern_fill_strip(uint8_t *buffer,
                                       size_t buffer_bytes,
                                       uint16_t y_start,
                                       uint16_t height)
{
    if (buffer == NULL || height == 0 || height > PCB_V1_LCD_STRIP_HEIGHT ||
        y_start >= PCB_V1_LCD_HEIGHT ||
        (uint32_t)y_start + height > PCB_V1_LCD_HEIGHT ||
        buffer_bytes < (size_t)PCB_V1_LCD_WIDTH * height * PCB_V1_LCD_BYTES_PER_PIXEL) {
        return;
    }

    size_t offset = 0;
    for (uint16_t y = y_start; y < y_start + height; ++y) {
        for (uint16_t x = 0; x < PCB_V1_LCD_WIDTH; ++x) {
            const uint16_t color = pattern_color(x, y);
            buffer[offset++] = (uint8_t)(color >> 8);
            buffer[offset++] = (uint8_t)(color & 0xFF);
        }
    }
}
