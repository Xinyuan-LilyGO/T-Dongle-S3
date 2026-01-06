/**
 * @file      blow_detector.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2026  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2026-01-06
 *
 */
#include <driver/I2S.h>
#include <math.h>
#include <Arduino.h>

#define SAMPLE_RATE                 8000

#define MIC_DATA                    8
#define MIC_SCK                     9

class BlowDetector
{
public:
    BlowDetector() {}

    bool begin()
    {
        static i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
            .sample_rate =  SAMPLE_RATE,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_PCM_SHORT,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 6,
            .dma_buf_len = 512,
            .use_apll = true
        };

        static i2s_pin_config_t i2s_cfg = {0};
        i2s_cfg.bck_io_num   = I2S_PIN_NO_CHANGE;
        i2s_cfg.ws_io_num    = MIC_SCK;
        i2s_cfg.data_out_num = I2S_PIN_NO_CHANGE;
        i2s_cfg.data_in_num  = MIC_DATA;
        i2s_cfg.mck_io_num = I2S_PIN_NO_CHANGE;

        if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
            Serial.println("i2s_driver_install error");
            return false;
        }
        if (i2s_set_pin(I2S_NUM_0, &i2s_cfg) != ESP_OK) {
            Serial.println("i2s_set_pin error");
            return false;
        }
        return true;
    }

    int getAudioLevel()
    {
        static int16_t buffer[256];

        size_t bytesRead = 0;
        i2s_read(I2S_NUM_0, (void*)buffer, sizeof(buffer), &bytesRead, portMAX_DELAY);

        if (bytesRead > 0) {
            int samples = bytesRead / sizeof(int16_t);
            if (samples > 0) {
                long sum = 0;
                for (int i = 0; i < samples; i++) {
                    sum += abs(buffer[i]);
                }
                return sum / samples;
            }
        }
        return 0;
    }
};
