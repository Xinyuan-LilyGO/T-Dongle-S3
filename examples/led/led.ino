/**
 * @file      led.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-15
 *
 */

#include <FastLED.h>
#include <Arduino.h>

#define LED_DI_PIN     40
#define LED_CI_PIN     39

CRGB leds[1];
CRGB colors[4] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};
uint32_t interval = 0;

void setup()
{
    // Tools-> USB CDC On boot -> Disable
    Serial.begin(115200);

    Serial.println("Start T-Dongle-S3 LED example");

    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(leds, 1);  // BGR ordering is typical
    FastLED.setBrightness(100);

    int i = 4;
    while (i > 0) {
        leds[0] = colors[4 - i];
        FastLED.show();
        delay(300);
        i--;
    }
}

void loop()
{
    if (millis() > interval) {
        leds[0] = CRGB(random(0, 255), random(0, 255), random(0, 255));
        FastLED.show();
        interval = millis() + random(100, 1000);
    }
    delay(5);
}