"""
fonts.py

    Cycles through all characters of four bitmap fonts on the display

"""

import utime
import st7735
import tft_config
import vga1_8x8 as font1
import vga1_8x16 as font2
import vga1_bold_16x16 as font3
import vga1_bold_16x32 as font4


tft = tft_config.config(1)


def main():
#   tft.init(st7735.INITR_GREENTAB)
#   tft.init(st7735.INITR_REDTAB)
#   tft.init(st7735.INITR_BLACKTAB)
#   tft.init(st7735.INITR_GREENTAB2)
#   tft.init(st7735.INITR_GREENTAB3)
#   tft.init(st7735.INITR_GREENTAB4)
    tft.init(st7735.INITR_GREENTAB5)
#   tft.init(st7735.INITB)


    while True:
        for font in (font1, font2, font3, font4):
            tft.fill(st7735.BLUE)
            line = 0
            col = 0
            for char in range(font.FIRST, font.LAST):
                tft.text(font, chr(char), col, line, st7735.WHITE, st7735.BLUE)
                col += font.WIDTH
                if col > tft.width() - font.WIDTH:
                    col = 0
                    line += font.HEIGHT

                    if line > tft.height()-font.HEIGHT:
                        utime.sleep(3)
                        tft.fill(st7735.BLUE)
                        line = 0
                        col = 0

            utime.sleep(3)

main()
