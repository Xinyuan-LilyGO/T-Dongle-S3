from machine import Pin, SPI
import st7735

TFA = 0
BFA = 0

def config(rotation=0, buffer_size=0, options=0):
    return st7735.ST7735(
        SPI(2, baudrate=30000000, sck=Pin(5), mosi=Pin(3)),
        80,
        160,
        reset=Pin(1, Pin.OUT),
        cs=Pin(4, Pin.OUT),
        dc=Pin(2, Pin.OUT),
        backlight=Pin(37, Pin.OUT),
        color_order=st7735.BGR,
        inversion=True,
        rotation=rotation,
        options=options,
        buffer_size=buffer_size)
