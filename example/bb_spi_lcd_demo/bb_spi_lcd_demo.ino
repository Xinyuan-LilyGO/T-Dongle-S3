//
// Hello World example which shows the simple way to use named displays
// of the bb_spi_lcd library to work with a variety of popular IoT products
// With only the name, bb_spi_lcd knows the complete LCD configuration
// (GPIO connections, interface, chip type), and can initialize+use it.
// Have a look in bb_spi_lcd.h for the complete list of more than 40 built-in
// display configurations of popular products. Multiple instances of the
// bb_spi_lcd class can control different display types simultaneously.
//
// This demo shows how to get started with custom fonts and colors
// The font is a TrueType font that was converted to Adafruit_GFX bitmap format
// with their conversion tool.
//
// written by Larry Bank (bitbank@pobox.com)
//
#include <bb_spi_lcd.h> // https://github.com/bitbank2/bb_spi_lcd
#include "Roboto_Black_40.h"

const uint16_t usColors[8] = {0x7e0, 0x67e0, 0xffe0, 0xfe40, 0xfbc0, 0xf800, 0xf81f, 0xffff};

BB_SPI_LCD lcd;

void setup()
{
  lcd.begin(DISPLAY_T_DONGLE_S3); // initialize the display and backlight
  lcd.setRotation(270); // The native LCD size is 80 wide, 160 tall, switch to 160x80
  lcd.fillScreen(TFT_BLACK);
  lcd.setFreeFont(&Roboto_Black_40);
} /* setup() */

void loop()
{
int iColor = 0;

  while (1) { // run forever
    lcd.setTextColor(usColors[iColor & 7]); // select 1 of the 8 colors
    iColor++; // increment the color for the next time through the loop
    lcd.setCursor(0, 40); // TrueType fonts use the baseline as the starting Y value
    lcd.print("Hello"); // the print() and printf() functionality is fully supported in bb_spi_lcd
    lcd.setCursor(0, 80);
    lcd.print("World!");
    delay(500); // 1/2 second delay between each color change
  } // while (1)
} /* loop() */
