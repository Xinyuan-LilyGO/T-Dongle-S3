"""
noto_fonts Writes the names of three Noto fonts centered on the display
    using the font. The fonts were converted from True Type fonts using
    the font2bitmap utility.
"""

import st7735
import tft_config
import NotoSans_32 as noto_sans
import NotoSerif_32 as noto_serif
import NotoSansMono_32 as noto_mono


tft = tft_config.config(1, buffer_size=16*32*2)


def center(font, s, row, color=st7735.WHITE):
    screen = tft.width()                     # get screen width
    width = tft.write_len(font, s)           # get the width of the string
    if width and width < screen:             # if the string < display
        col = tft.width() // 2 - width // 2  # find the column to center
    else:                                    # otherwise
        col = 0                              # left justify

    tft.write(font, s, col, row, color)      # and write the string


def main():
    # init display
#     tft.init(st7735.INITR_GREENTAB)
#     tft.init(st7735.INITR_REDTAB)
#     tft.init(st7735.INITR_BLACKTAB)
#     tft.init(st7735.INITR_GREENTAB2)
#     tft.init(st7735.INITR_GREENTAB3)
#     tft.init(st7735.INITR_GREENTAB4)
    tft.init(st7735.INITR_GREENTAB5)
#     tft.init(st7735.INITB)

    tft.fill(st7735.BLACK)

    # center the name of the first font, using the font
    row = 16
    center(noto_sans, "NotoSans", row, st7735.RED)
    row += noto_sans.HEIGHT

    # center the name of the second font, using the font
    center(noto_serif, "NotoSerif", row, st7735.GREEN)
    row += noto_serif.HEIGHT

    # center the name of the third font, using the font
    center(noto_mono, "NotoSansMono", row, st7735.BLUE)
    row += noto_mono.HEIGHT


main()
