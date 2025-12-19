# How Flash

![Video](https://youtu.be/7Rfl1RMdX-w)

## Mac & Liunx
```
1. python3 -m esptool
2. esptool -p Your-Port   --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m  0 ./firmware/firmware.bin
```
## Win
```
1. pip install esptool
2. esptool.py.exe -p COM91   --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m  0 ./firmware/firmware.bin
```


