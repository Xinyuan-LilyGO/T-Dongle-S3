
#include "Arduino.h"
#include "SD_MMC.h"
#include "WiFi.h"
#include "logo.h"
#include "lv_driver.h"
#include "pin_config.h"

/* external library */
/* To use Arduino, you need to place lv_conf.h in the \Arduino\libraries directory */
#include "OneButton.h" // https://github.com/mathertel/OneButton
#include "TFT_eSPI.h"  // https://github.com/Bodmer/TFT_eSPI

#include "lv_conf.h"
#include "lvgl.h"    // https://github.com/lvgl/lvgl
#include <FastLED.h> // https://github.com/FastLED/FastLED

TFT_eSPI tft = TFT_eSPI();
CRGB leds;
OneButton button(BTN_PIN, true);
uint8_t btn_press = 0;
lv_obj_t *tv;
#define PRINT_STR(str, x, y)                                                                                                                         \
  do {                                                                                                                                               \
    Serial.println(str);                                                                                                                             \
    tft.drawString(str, x, y);                                                                                                                       \
    y += 8;                                                                                                                                          \
  } while (0);

void led_task(void *param) {
  while (1) {
    static uint8_t hue = 0;
    leds = CHSV(hue++, 0XFF, 100);
    FastLED.show();
    delay(50);
  }
}

void sd_init(void) {
  int32_t x, y;
  SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
  if (!SD_MMC.begin()) {
    PRINT_STR("Card Mount Failed", x, y)
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    PRINT_STR("No SD_MMC card attached", x, y)
    return;
  }
  String str;
  str = "SD_MMC Card Type: ";
  if (cardType == CARD_MMC) {
    str += "MMC";
  } else if (cardType == CARD_SD) {
    str += "SD_MMCSC";
  } else if (cardType == CARD_SDHC) {
    str += "SD_MMCHC";
  } else {
    str += "UNKNOWN";
  }

  PRINT_STR(str, x, y)
  uint32_t cardSize = SD_MMC.cardSize() / (1024 * 1024);

  str = "SD_MMC Card Size: ";
  str += cardSize;
  PRINT_STR(str, x, y)

  str = "Total space: ";
  str += uint32_t(SD_MMC.totalBytes() / (1024 * 1024));
  str += "MB";
  PRINT_STR(str, x, y)

  str = "Used space: ";
  str += uint32_t(SD_MMC.usedBytes() / (1024 * 1024));
  str += "MB";
  PRINT_STR(str, x, y)
}

void scan_wifi_rssi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  WiFi.mode(WIFI_OFF);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello T-Dongle-S3");
  pinMode(TFT_LEDA_PIN, OUTPUT);
  // Initialise TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_DARKGREY);
  digitalWrite(TFT_LEDA_PIN, 0);
  tft.setTextFont(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.pushImage(0, 0, 160, 80, (uint16_t *)gImage_logo);
  delay(2000);
  sd_init();
  delay(4000);
  scan_wifi_rssi();
  // BGR ordering is typical
  FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(&leds, 1);

  button.attachClick([] {
    btn_press = 1 ^ btn_press;
    lv_obj_set_tile_id(tv, 0, btn_press, LV_ANIM_ON);
  });

  xTaskCreatePinnedToCore(led_task, "led_task", 1024, NULL, 1, NULL, 0);

  lvgl_init();

  tv = lv_tileview_create(lv_scr_act());
  lv_obj_remove_style(tv, NULL, LV_PART_SCROLLBAR);

  /* just show the logo gif */
  lv_obj_t *tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_VER);
  LV_IMG_DECLARE(img_src_logo);
  lv_obj_t *img = lv_gif_create(tile1);
  lv_obj_set_size(img, 160, 80);
  lv_gif_set_src(img, &img_src_logo);
  lv_obj_center(img);

  lv_obj_t *tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_VER);
  lv_obj_t *label = lv_label_create(tile2);
  String text;
  esp_chip_info_t t;
  esp_chip_info(&t);
  text = "chip:ESP32-S3\n";
  text += "Hello T-Dongle-S3";
  lv_label_set_text(label, text.c_str());
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void loop() { // Put your main code here, to run repeatedly:
  lv_timer_handler();
  button.tick();
  delay(5);
}
