LILYGO T-Dongle-S3
==

Example code for the [t-dongle-s3](https://www.lilygo.cc/products/t-dongle-s3) using esp-idf.
The original [factory example](https://github.com/Xinyuan-LilyGO/T-Dongle-S3/tree/main/example/Factory) from LILYGO is made for Arduino


usage:

```
git clone --recurse-submodules <this repository>
idf.py set-target esp32s3
idf.py build flash monitor
```

The code has been kept to a minimum, showing how to use:
- the button and the RGB led, using the [esp-idf-lib from UncleRus](https://github.com/UncleRus/esp-idf-lib/)
- the display using [LovyanGFX from lovyan03](https://github.com/lovyan03/LovyanGFX/)
- the microsd slot using the esp-idf [SDMMC driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdmmc_host.html)
