/**
 * @file      microphone.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-15
 *
 */

#include <Arduino.h>

#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3,0,0)
#error This example requires Arduino-ESP32 version 3.0.0 or higher
void setup() {}
void loop() {}
#else
#include <ESP_I2S.h>
#include <FS.h>
#include <SD_MMC.h>

// SD card pins
#define SD_MMC_D0_PIN   14
#define SD_MMC_D1_PIN   17
#define SD_MMC_D2_PIN   21
#define SD_MMC_D3_PIN   18
#define SD_MMC_CLK_PIN  12
#define SD_MMC_CMD_PIN  16
// PDM microphone pins
#define PDM_SCK         9
#define PDM_DATA        8

// Create an instance of the I2SClass
I2SClass i2s;

void setup()
{
    // Create variables to store the audio data
    uint8_t *wav_buffer;
    size_t wav_size;

    // Initialize the serial port
    Serial.begin(115200);

    Serial.println("Initializing I2S bus...");

    // Set up the pins used for audio input
    i2s.setPinsPdmRx(PDM_SCK, PDM_DATA);

    // Initialize the I2S bus in PDM RX mode
    if (!i2s.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT)) {
        Serial.println("Failed to initialize I2S bus!");
        return;
    }

    Serial.println("I2S bus initialized.");
    Serial.println("Initializing SD card...");
    // Set up the pins used for SD card access
    SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
    // Mount the SD card
    if (!SD_MMC.begin()) {
        Serial.println("Failed to initialize SD card!");
        return;
    }

    Serial.println("SD card initialized.");
    Serial.println("Recording 5 seconds of audio data...");

    // Record 5 seconds of audio data
    wav_buffer = i2s.recordWAV(5, &wav_size);

    // Create a file on the SD card
    File file = SD_MMC.open("/test.wav", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing!");
        return;
    }

    Serial.println("Writing audio data to file...");

    // Write the audio data to the file
    if (file.write(wav_buffer, wav_size) != wav_size) {
        Serial.println("Failed to write audio data to file!");
        return;
    }

    // Close the file
    file.close();

    Serial.println("Application complete.");
}

void loop() {}
#endif