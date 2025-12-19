/**
 * @file      usb_mass_storage.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-15
 * @note      Arduino IDE -> Tools -> USB Mode -> Select "USB-OTG (TinyUSB)" mode before using this sketch
 */
#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "esp_arduino_version.h"
#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 0)
#error This sketch requires ESP32 Arduino Core version 3.3.0 or later
#endif

#include "USB.h"
#include "USBMSC.h"
#include "SD_MMC.h"

#define SD_MMC_D0_PIN  14
#define SD_MMC_D1_PIN  17
#define SD_MMC_D2_PIN  21
#define SD_MMC_D3_PIN  18
#define SD_MMC_CLK_PIN 12
#define SD_MMC_CMD_PIN 16

USBMSC MSC;

static  uint32_t DISK_SECTOR_COUNT = 0;
static  uint16_t DISK_SECTOR_SIZE = 0;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    uint32_t secSize = SD_MMC.sectorSize();
    if (!secSize) {
        return 0;  // disk error
    }
    for (int x = 0; x < bufsize / secSize; x++) {
        uint8_t blkbuffer[secSize];
        memcpy(blkbuffer, (uint8_t *)buffer + secSize * x, secSize);
        if (!SD_MMC.writeRAW(blkbuffer, lba + x)) {
            return 0;
        }
    }
    return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    uint32_t secSize = SD_MMC.sectorSize();
    if (!secSize) {
        return false;  // disk error
    }
    for (int x = 0; x < bufsize / secSize; x++) {
        if (!SD_MMC.readRAW((uint8_t *)buffer + (x * secSize), lba + x)) {
            return 0;  // outside of volume boundary
        }
    }
    return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
    Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
    return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ARDUINO_USB_EVENTS) {
        arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
        switch (event_id) {
        case ARDUINO_USB_STARTED_EVENT: Serial.println("USB PLUGGED"); break;
        case ARDUINO_USB_STOPPED_EVENT: Serial.println("USB UNPLUGGED"); break;
        case ARDUINO_USB_SUSPEND_EVENT: Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en); break;
        case ARDUINO_USB_RESUME_EVENT:  Serial.println("USB RESUMED"); break;
        default: break;
        }
    }
}

void setup()
{
    Serial.begin(115200);

    //Note:  Arduino IDE -> Tools -> USB Mode -> Select "USB-OTG (TinyUSB)" mode before using this sketch

    SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);

    if (!SD_MMC.begin()) {
        while (1) {
            Serial.println("SD Card Mount Failed");
            delay(1000);
        }
    }

    USB.onEvent(usbEventCallback);
    MSC.vendorID("ESP32");       //max 8 chars
    MSC.productID("USB_MSC");    //max 16 chars
    MSC.productRevision("1.0");  //max 4 chars
    MSC.onStartStop(onStartStop);
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);

    MSC.mediaPresent(true);
    // MSC.isWritable(true);  // true if writable, false if read-only

    DISK_SECTOR_COUNT = SD_MMC.numSectors();
    DISK_SECTOR_SIZE =  SD_MMC.sectorSize();

    MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
    USB.begin();

}

void loop()
{
    // put your main code here, to run repeatedly:
}
#endif /* ARDUINO_USB_MODE */
