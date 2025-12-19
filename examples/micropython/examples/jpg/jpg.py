"""
jpg.py

    Draw a full screen jpg using the slower but less memory intensive method of blitting
    each Minimum Coded Unit (MCU) block. Usually 8×8pixels but can be other multiples of 8.

    bigbuckbunny.jpg (c) copyright 2008, Blender Foundation / www.bigbuckbunny.org
"""

import random
import st7735
import tft_config

tft = tft_config.config(0)

def main():
    '''
    Decode and draw jpg on display
    '''
#     tft.init(st7735.INITR_GREENTAB)
#     tft.init(st7735.INITR_REDTAB)
#     tft.init(st7735.INITR_BLACKTAB)
#     tft.init(st7735.INITR_GREENTAB2)
#     tft.init(st7735.INITR_GREENTAB3)
#     tft.init(st7735.INITR_GREENTAB4)
    tft.init(st7735.INITR_GREENTAB5)
#     tft.init(st7735.INITB)
#     tft.jpg(f'bigbuckbunny-{tft.width()}x{tft.height()}.jpg', 0, 0, st7735.SLOW)
    tft.jpg(f'bigbuckbunny-80x160.jpg', 0, 0, st7735.SLOW)
main()
