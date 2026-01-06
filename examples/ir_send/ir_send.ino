/**
 * @file      ir_send.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-15
 *
 */
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define PIN_NUM_IR_SEND    7


IRsend irsend(PIN_NUM_IR_SEND);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[67] = {9000, 4500, 650, 550, 650, 1650, 600, 550, 650, 550,
                        600, 1650, 650, 550, 600, 1650, 650, 1650, 650, 1650,
                        600, 550, 650, 1650, 650, 1650, 650, 550, 600, 1650,
                        650, 1650, 650, 550, 650, 550, 650, 1650, 650, 550,
                        650, 550, 650, 550, 600, 550, 650, 550, 650, 550,
                        650, 1650, 600, 550, 650, 1650, 650, 1650, 650, 1650,
                        650, 1650, 650, 1650, 650, 1650, 600
                       };
// Example Samsung A/C state captured from IRrecvDumpV2.ino
uint8_t samsungState[kSamsungAcStateLength] = {
    0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0,
    0x01, 0xE2, 0xFE, 0x71, 0x40, 0x11, 0xF0
};

void setup()
{
    Serial.begin(115200);
    irsend.begin();
}

void loop()
{
    Serial.println("NEC");
    irsend.sendNEC(0x00FFE01FUL);
    delay(2000);
    Serial.println("Sony");
    irsend.sendSony(0xa90, 12, 2);  // 12 bits & 2 repeats
    delay(2000);
    Serial.println("a rawData capture from IRrecvDumpV2");
    irsend.sendRaw(rawData, 67, 38);  // Send a raw data capture at 38kHz.
    delay(2000);
    Serial.println("a Samsung A/C state from IRrecvDumpV2");
    irsend.sendSamsungAC(samsungState);
    delay(2000);
}
