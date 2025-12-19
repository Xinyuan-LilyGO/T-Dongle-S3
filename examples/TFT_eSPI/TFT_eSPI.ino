
#include "Arduino.h"
#include "girl.h"
#include "logo.h"
#include "logo2.h"
#include "TFT_eSPI.h" // https://github.com/Bodmer/TFT_eSPI


#define LED_DI_PIN     40
#define LED_CI_PIN     39

#define TFT_CS_PIN     4
#define TFT_SDA_PIN    3
#define TFT_SCL_PIN    5
#define TFT_DC_PIN     2
#define TFT_RES_PIN    1
#define TFT_LEDA_PIN   38

#define SD_MMC_D0_PIN  14
#define SD_MMC_D1_PIN  17
#define SD_MMC_D2_PIN  21
#define SD_MMC_D3_PIN  18
#define SD_MMC_CLK_PIN 12
#define SD_MMC_CMD_PIN 16

TFT_eSPI tft = TFT_eSPI();

void setup()
{
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

void loop()   // Put your main code here, to run repeatedly:
{
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
