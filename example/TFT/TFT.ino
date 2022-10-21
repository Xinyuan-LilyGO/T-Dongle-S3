
#include "Arduino.h"
#include "girl.h"
#include "logo.h"
#include "logo2.h"
#include "pin_config.h"

/* external library */
/* To use Arduino, you need to place lv_conf.h in the \Arduino\libraries directory */
#include "TFT_eSPI.h" // https://github.com/Bodmer/TFT_eSPI

TFT_eSPI tft = TFT_eSPI();

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
}

void loop() { // Put your main code here, to run repeatedly:
  static uint8_t i;
  switch (i++) {
  case 0:
    tft.pushImage(0, 0, 160, 80, (uint16_t *)gImage_logo);
    break;
  case 1:
    tft.pushImage(0, 0, 160, 80, (uint16_t *)gImage_logo2);
    break;
  case 2:
    tft.pushImage(0, 0, 160, 80, (uint16_t *)gImage_girl);
    i = 0;
    break;
  default:
    break;
  }
  delay(2000);
}
