/**
 * @file      lvgl9.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-15
 *
 */
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_st7735.h"
#include <Arduino.h>
#include <lvgl.h>

#define LCD_HOST                    SPI2_HOST
#define PIN_NUM_MISO                -1
#define PIN_NUM_MOSI                3
#define PIN_NUM_CLK                 5
#define PIN_NUM_CS                  4
#define PIN_NUM_DC                  2
#define PIN_NUM_RST                 1
#define PIN_NUM_BCKL                38

#define LCD_PIXEL_WIDTH             160
#define LCD_PIXEL_HEIGHT            80
#define LCD_CMD_BITS                8
#define LCD_PARAM_BITS              8
#define LCD_BK_LIGHT_ON             0
#define LCD_BK_LIGHT_OFF            1
#define LEDC_BACKLIGHT_FREQ         1000
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_CHANNEL      3

static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;
static spi_device_handle_t spi_handle = NULL;
static esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = PIN_NUM_RST,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    .color_space = ESP_LCD_COLOR_SPACE_BGR,
#else
    .color_space = LCD_RGB_ELEMENT_ORDER_BGR,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
#endif
    .bits_per_pixel = 16,
};
static spi_bus_config_t spi_config = ST7735_PANEL_BUS_SPI_CONFIG(
        PIN_NUM_CLK, PIN_NUM_MOSI, 160 * 80 * sizeof(uint16_t));

static esp_lcd_panel_io_spi_config_t io_config = ST7735_PANEL_IO_SPI_CONFIG(
        PIN_NUM_CS, PIN_NUM_DC, NULL, NULL);

uint16_t disp_width = LCD_PIXEL_WIDTH;
uint16_t disp_height = LCD_PIXEL_HEIGHT;

static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
    size_t len = lv_area_get_size(area);
    lv_draw_sw_rgb565_swap(color_p, len);
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, (uint16_t *)color_p);
    lv_display_flush_ready(disp);
}

static uint32_t lv_tick_get_callback(void)
{
    return millis();
}

void setup()
{
    Serial.begin(115200);

    Serial.println("Hello T-Dongle-S3");

    // ================= Initialize SPI bus =================
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &spi_config, SPI_DMA_CH_AUTO));
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    log_d( "Install ST7735 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));

    // When in landscape mode, set the spacing.
    esp_lcd_panel_set_gap(panel_handle, 1, 26);
    esp_lcd_panel_swap_xy(panel_handle, true);
    disp_width = LCD_PIXEL_WIDTH;
    disp_height = LCD_PIXEL_HEIGHT;
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, true));

    // When using portrait mode, set the spacing.
    // esp_lcd_panel_set_gap(panel_handle, 26, 1);
    // esp_lcd_panel_swap_xy(panel_handle, false);
    // disp_width = LCD_PIXEL_HEIGHT;
    // disp_height = LCD_PIXEL_WIDTH;
    // ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, true));

    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    Serial.println("LCD initialized");

    // ======================= Initialize LVGL =======================
    lv_init();

    Serial.println("LVGL initialized");
    size_t lv_buffer_size = disp_width * disp_height * sizeof(lv_color16_t);
    lv_color16_t *buf = (lv_color16_t *)malloc(lv_buffer_size);
    assert(buf);
    lv_display_t *disp_drv = lv_display_create(disp_width, disp_height);
    lv_display_set_buffers(disp_drv, buf, NULL, lv_buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(disp_drv, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp_drv, disp_flush);
    lv_tick_set_cb(lv_tick_get_callback);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello LVGL9 on\nT-Dongle-S3!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // ======================= Initialize Backlight =======================

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
    ledcAttach(PIN_NUM_BCKL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
#else
    ledcSetup(LEDC_BACKLIGHT_CHANNEL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
    ledcAttachPin(PIN_NUM_BCKL, LEDC_BACKLIGHT_CHANNEL);
#endif

    ledcWrite(LEDC_BACKLIGHT_CHANNEL, 0); // Set backlight to maximum brightness
}

void loop()
{
    lv_timer_handler();
    delay(5);
}