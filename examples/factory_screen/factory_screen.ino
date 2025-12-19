/**
 * @file      factory_screen.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-18
 *
 */
#include <Arduino.h>
#include "SD_MMC.h"
#include "WiFi.h"
#include <lvgl.h>
#include <FastLED.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_st7735.h"

#define WIFI_SSID            "Your_SSID"
#define WIFI_PASSWORD        "Your_PASSWORD"

#ifndef BOOT_PIN
#define BOOT_PIN       0
#endif

#define LED_DI_PIN     40
#define LED_CI_PIN     39

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

#define SD_MMC_D0_PIN               14
#define SD_MMC_D1_PIN               17
#define SD_MMC_D2_PIN               21
#define SD_MMC_D3_PIN               18
#define SD_MMC_CLK_PIN              12
#define SD_MMC_CMD_PIN              16

#define LV_SCREEN_WIDTH             160
#define LV_SCREEN_HEIGHT            80

LV_FONT_DECLARE(lv_font_maplemomo_12);
LV_FONT_DECLARE(lv_font_maplemomo_14);
LV_FONT_DECLARE(lv_font_maplemomo_16);

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

uint32_t rgb_change_interval = 0; // milliseconds
uint8_t  rgb_hue = 0;
uint16_t disp_width = LCD_PIXEL_WIDTH;
uint16_t disp_height = LCD_PIXEL_HEIGHT;
lv_obj_t *tileview;
CRGB leds;
bool vertical_screen = true;

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

    pinMode(BOOT_PIN, INPUT);

    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(&leds, 1);
    FastLED.setBrightness(100);

    // ================ Initialize SD card =================
    SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
    if (!SD_MMC.begin()) {
        Serial.println("Card Mount Failed");
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType != CARD_NONE) {
        Serial.print("SD_MMC Card Type: ");
        if (cardType == CARD_MMC) {
            Serial.println("MMC");
        } else if (cardType == CARD_SD) {
            Serial.println("SDSC");
        } else if (cardType == CARD_SDHC) {
            Serial.println("SDHC");
        } else {
            Serial.println("UNKNOWN");
        }
    }

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


    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    vertical_screen = disp_width == LCD_PIXEL_HEIGHT;

    Serial.println("LCD initialized");

    // ======================= Initialize LVGL =======================
    lv_init();

    Serial.println("LVGL initialized");
    size_t lv_buffer_size = disp_width * disp_height* sizeof(lv_color16_t);
    lv_color16_t *buf = (lv_color16_t *)malloc(lv_buffer_size);
    assert(buf);
    lv_display_t *disp_drv = lv_display_create(disp_width, disp_height);
    lv_display_set_buffers(disp_drv, buf, NULL, lv_buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(disp_drv, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp_drv, disp_flush);
    lv_tick_set_cb(lv_tick_get_callback);


    // ======================== Show Factory Info =======================

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);

    tileview = lv_tileview_create(lv_screen_active());
    lv_obj_set_style_bg_color(tileview, lv_color_black(), 0);
    lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR);

    lv_obj_t *tile1 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_VER);
    lv_obj_t *tile2 = lv_tileview_add_tile(tileview, 0, 1, LV_DIR_VER);

    LV_IMG_DECLARE(img_t_dongle_s3_ce_fcc);
    lv_obj_t *img = lv_image_create(tile1);
    lv_image_set_src(img, &img_t_dongle_s3_ce_fcc);
    lv_obj_center(img);

    lv_obj_t *cont = lv_obj_create(tile2);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(cont, 5, 0);
    lv_obj_set_style_border_color(cont, lv_color_make(69, 163, 76), 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *rssi_label;

    if (vertical_screen) {

        lv_obj_set_style_pad_left(cont, 1, 0);
        lv_obj_set_style_pad_right(cont, 1, 0);

        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, "LilyGo-T-Dongle-S3");
        lv_obj_set_style_text_font(label, &lv_font_maplemomo_14, 0);
        lv_obj_set_width(label, lv_pct(100));
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        lv_obj_t *sd_size_label = lv_label_create(cont);
        lv_label_set_text(sd_size_label, "SD Card: ");
        lv_obj_set_style_text_font(sd_size_label, &lv_font_maplemomo_12, 0);
        lv_obj_set_style_text_align(sd_size_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align_to(sd_size_label, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

        lv_obj_t *size_label = lv_label_create(cont);
        lv_obj_set_style_text_font(size_label, &lv_font_maplemomo_16, 0);
        lv_obj_set_style_text_align(size_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align_to(size_label, sd_size_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        if (cardType != CARD_NONE) {
            uint64_t card_size_bytes = SD_MMC.cardSize();
            float sdCardSize;
            if (card_size_bytes >= (uint64_t)1024 * 1024 * 1024) {
                sdCardSize = (float)card_size_bytes / (1024.0 * 1024.0 * 1024.0);
                lv_label_set_text_fmt(size_label, "%.0fGB", sdCardSize);
            } else {
                sdCardSize = (float)card_size_bytes / (1024.0 * 1024.0);
                lv_label_set_text_fmt(size_label, "%.0fMB", sdCardSize);
            }
        } else {
            lv_label_set_text(size_label,  "N.A");
        }

        lv_obj_t *wifi_label = lv_label_create(cont);
        lv_label_set_text(wifi_label, "WiFi Rssi:");
        lv_obj_set_style_text_font(wifi_label, &lv_font_maplemomo_12, 0);
        lv_obj_set_style_text_align(wifi_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align_to(wifi_label, size_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


        rssi_label = lv_label_create(cont);
        lv_label_set_text(rssi_label, "-39dBm");
        lv_obj_set_style_text_font(rssi_label, &lv_font_maplemomo_14, 0);
        lv_obj_set_style_text_align(rssi_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align_to(rssi_label, wifi_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    } else {

        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, "LilyGo-T-Dongle-S3");
        lv_obj_set_style_text_font(label, &lv_font_maplemomo_14, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, -8);

        lv_obj_t *sd_size_label = lv_label_create(cont);
        lv_label_set_text(sd_size_label, "SD Card: ");
        lv_obj_set_style_text_font(sd_size_label, &lv_font_maplemomo_12, 0);
        lv_obj_set_style_text_align(sd_size_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(sd_size_label, LV_ALIGN_TOP_LEFT, -5, 10);

        lv_obj_t *size_label = lv_label_create(cont);
        lv_obj_set_style_text_font(size_label, &lv_font_maplemomo_16, 0);
        lv_obj_set_style_text_align(size_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(size_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        if (cardType != CARD_NONE) {
            uint64_t card_size_bytes = SD_MMC.cardSize();
            float sdCardSize;
            if (card_size_bytes >= (uint64_t)1024 * 1024 * 1024) {
                sdCardSize = (float)card_size_bytes / (1024.0 * 1024.0 * 1024.0);
                lv_label_set_text_fmt(size_label, "%.0fGB", sdCardSize);
            } else {
                sdCardSize = (float)card_size_bytes / (1024.0 * 1024.0);
                lv_label_set_text_fmt(size_label, "%.0fMB", sdCardSize);
            }
        } else {
            lv_label_set_text(size_label,  "N.A");
        }

        lv_obj_t *wifi_label = lv_label_create(cont);
        lv_label_set_text(wifi_label, "WiFi Rssi:");
        lv_obj_set_style_text_font(wifi_label, &lv_font_maplemomo_12, 0);
        lv_obj_set_style_text_align(wifi_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(wifi_label, LV_ALIGN_TOP_RIGHT, 5, 10);

        rssi_label = lv_label_create(cont);
        lv_label_set_text(rssi_label, "-39dBm");
        lv_obj_set_style_text_font(rssi_label, &lv_font_maplemomo_16, 0);
        lv_obj_set_style_text_align(rssi_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(rssi_label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);


    }

    lv_timer_create([](lv_timer_t * timer) {
        lv_obj_t *label = (lv_obj_t *)lv_timer_get_user_data(timer);
        lv_label_set_text_fmt(label, "%ddBm", WiFi.RSSI());
    }, 2000, rssi_label);
    // ======================= Initialize Backlight =======================

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
    ledcAttach(PIN_NUM_BCKL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
#else
    ledcSetup(LEDC_BACKLIGHT_CHANNEL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
    ledcAttachPin(PIN_NUM_BCKL, LEDC_BACKLIGHT_CHANNEL);
#endif

    ledcWrite(LEDC_BACKLIGHT_CHANNEL, 0); // Set backlight to maximum brightness


    // Auto switch to the 2nd tile after 3 seconds
    lv_timer_create([](lv_timer_t * timer) {
        lv_tileview_set_tile_by_index(tileview, 0, 1, LV_ANIM_ON);
        lv_timer_del(timer);
    }, 3000, NULL);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}


void loop()   // Put your main code here, to run repeatedly:
{
    static uint8_t btn_press = 0;

    if (digitalRead(BOOT_PIN) == LOW) {
        btn_press = 1 ^ btn_press;
        lv_tileview_set_tile_by_index(tileview, 0, btn_press, LV_ANIM_ON);
        while (digitalRead(BOOT_PIN) == LOW) {
            delay(10);
        }
    }
    if (millis() > rgb_change_interval) {
        rgb_change_interval = millis() + 50;
        leds = CHSV(rgb_hue++, 0XFF, 100);
        FastLED.show();
    }

    lv_timer_handler();
    delay(5);
}
