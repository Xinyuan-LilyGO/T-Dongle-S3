"""
hello.py

    Writes "Hello!" in random colors at random locations on the display.
"""

import random
import utime
import st7735
import tft_config
import vga1_bold_16x32 as font

tft = tft_config.config(1)

def center(text):
    length = len(text)
    tft.text(
        font,
        text,
        tft.width() // 2 - length // 2 * font.WIDTH,
        tft.height() // 2 - font.HEIGHT,
        st7735.WHITE,
        st7735.RED)

def main():
#     tft.init(st7735.INITR_GREENTAB)
#     tft.init(st7735.INITR_REDTAB)
#     tft.init(st7735.INITR_BLACKTAB)
#     tft.init(st7735.INITR_GREENTAB2)
#     tft.init(st7735.INITR_GREENTAB3)
#     tft.init(st7735.INITR_GREENTAB4)
#     tft.init(st7735.INITR_GREENTAB5)
#     tft.init(st7735.INITB)
    tft.init(st7735.INITR_GREENTAB5)

    tft.fill(st7735.RED)
    center("Hello!")
    utime.sleep(2)
    tft.fill(st7735.BLACK)

    while True:
        for rotation in range(4):
            tft.rotation(rotation)
            tft.fill(0)
            col_max = tft.width() #- font.WIDTH*6 #
            row_max = tft.height() #- font.HEIGHT

            for _ in range(128):
                tft.text(
                    font,
                    "Hello!",
                    random.randint(0, col_max),
                    random.randint(0, row_max),
                    st7735.color565(
                        random.getrandbits(8),
                        random.getrandbits(8),
                        random.getrandbits(8)),
                    st7735.color565(
                        random.getrandbits(8),
                        random.getrandbits(8),
                        random.getrandbits(8)))


main()
