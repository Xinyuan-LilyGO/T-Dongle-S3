:start
esptool.exe --chip esp32s3   --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0 firmware.bin

@echo Press any key,keey find device praogram
pause
goto start

