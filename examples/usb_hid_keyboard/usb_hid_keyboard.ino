/**
 * @file      usb_hid_keyboard.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-15
 * @note      Arduino IDE -> Tools -> USB Mode -> Select "USB-OTG (TinyUSB)" mode before using this sketch
 */

#if ARDUINO_USB_MODE == 1
#error Arduino IDE -> Tools -> USB Mode -> Select "USB-OTG (TinyUSB)" mode before using this sketch
void setup() {}
void loop() {}
#else
#include "esp_arduino_version.h"
#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 0)
#error This sketch requires ESP32 Arduino Core version 3.3.0 or later
#endif

#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup()
{
    pinMode(BOOT_PIN, INPUT_PULLUP);
    Keyboard.begin();
    USB.begin();
}

void loop()
{
    if (digitalRead(BOOT_PIN) == LOW) {
        Keyboard.write('u');
    }
    delay(5);
}
#endif /* ARDUINO_USB_MODE */
