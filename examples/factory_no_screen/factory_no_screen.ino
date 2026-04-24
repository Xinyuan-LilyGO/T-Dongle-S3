/**
 * @file      factory_no_screen.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-17
 *
 */
#ifndef BOOT_PIN
#define BOOT_PIN       0
#endif
#define LED_DI_PIN     40
#define LED_CI_PIN     39
#define SD_MMC_D0_PIN  14
#define SD_MMC_D1_PIN  17
#define SD_MMC_D2_PIN  21
#define SD_MMC_D3_PIN  18
#define SD_MMC_CLK_PIN 12
#define SD_MMC_CMD_PIN 16

#include <Arduino.h>
#include <FastLED.h>
#include <FS.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#ifndef WIFI_SSID
#define WIFI_SSID            "Your_SSID"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD        "Your_PASSWORD"
#endif

CRGB leds = CRGB::Green;
CRGB colors[4] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};
uint32_t rssi_measure_time = 0;
uint64_t sdCardSize;
WiFiMulti wifiMulti;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Factory No Screen Example");

    pinMode(BOOT_PIN, INPUT);

    // Initialize FastLED
    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(&leds, 1);  // BGR ordering is typical
    FastLED.setBrightness(100);
    Serial.println("FastLED initialized with no screens.");
    FastLED.show();

    // Initialize SD card
    SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
    if (!SD_MMC.begin()) {
        Serial.println("SD Card Mount Failed");
    } else {
        uint8_t sdCardType = SD_MMC.cardType();
        if (sdCardType == CARD_NONE) {
            Serial.println("No SD Card attached");
        } else {
            Serial.println("SD Card initialized.");
            sdCardSize = SD_MMC.cardSize() / (1024 * 1024);
            Serial.printf("SD Card Size: %lluMB\n", sdCardSize);

            listDir(SD_MMC, "/", 1);
        }
    }

    // Initialize WiFi in Station mode
    WiFi.mode(WIFI_STA);
    Serial.println("WiFi set to Station mode.");
    Serial.println("Scan start");

    // Scan WiFi
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.printf("%2d", i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | ");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");
            switch (WiFi.encryptionType(i)) {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);
        }
    }
    Serial.println("");

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    Serial.print("Connecting to WiFi ");
    Serial.println(WIFI_SSID);
    // Serial.print("Password: ");
    // Serial.println(WIFI_PASSWORD);

    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Start WiFi without connecting to any network
    
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
#ifdef WIFI_SSID1
    wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
#endif

    // Wait for connection
    while (wifiMulti.run() != WL_CONNECTED ) {
        leds = CRGB(random(0, 255), random(0, 255), random(0, 255));
        FastLED.show();
        Serial.print(".");
        delay(100);
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    leds = CRGB::Green;
    FastLED.show();
}

void loop()
{
    if (digitalRead(BOOT_PIN) == LOW) {
        // leds = CRGB(random(0, 255), random(0, 255), random(0, 255));
        // FastLED.show();
        static uint8_t colors_index = 0;
        leds = colors[colors_index];
        Serial.printf("Button click , show color:%d\n", colors_index);
        FastLED.show();
        colors_index++;
        colors_index %= sizeof(colors) / sizeof(colors[0]);
        while (digitalRead(BOOT_PIN) == LOW) {
            delay(10);
        }
    }

    if (millis() > rssi_measure_time ) {
        int8_t  rssi = WiFi.RSSI();
        Serial.printf("WiFi RSSI: %d dBm SD Size:%lluMB\n", rssi, sdCardSize);
        rssi_measure_time = millis() + 1000; // Measure every 1 seconds
    }
}