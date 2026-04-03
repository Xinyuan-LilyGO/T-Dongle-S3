:start
esptool.exe --chip esp32s3   --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0 T-Dongle-S3-Plus_factory_screen_260106.bin

@echo Press any key, keep finding device program
pause
goto start

