/*
 * st7302.c
 *
 *  Created on: Jan 27, 2023
 *      Author: larry
 */
#include <stdint.h>
#include <string.h>
#include "Arduino.h"
#include "SPI.h"
#include "st7302.h"

static uint8_t u8DCPin, u8CSPin, u8RSTPin;
static uint8_t u8Cache[LCD_WIDTH * 2]; // 250 cols x 12 rows
static uint8_t cursor_x, cursor_y;

const uint8_t st7302_init[] = {
    0x02, 0xEB, 0x02, // Enable OTP
    0x02, 0xD7, 0x68, // OTP Load Control
    0x02, 0xD1, 0x01, // Auto Power Control
    0x02, 0xC0, 0x40, // Gate Voltage setting
    0x07, 0xC1, 0x22, 0x22, 0x22, 0x22, 0x14, 0x00, // VSH Setting
    0x05, 0xC2, 0x00, 0x00, 0x00, 0x00, // VSL Setting
    0x02, 0xCB, 0x0E, // VCOMH Setting
    0x0B, 0xB4, 0xE5, 0x66, 0x85, 0xFF, 0xFF, 0x52, 0x85, 0xFF, 0xFF, 0x52, // Gate EQ
    0x03, 0xC7, 0xA6, 0xE9, // OSC Setting
    0x02, 0xB0, 0x3F, // Duty Setting
    0x03, 0xB2, 0x00, 0x00, // Frame Rate Setting (lowest for both HPM/LPM)
    0x02, 0x36, 0x00, // Memory Access Mode
    0x02, 0x3A, 0x11, // Data Format
    0x02, 0xB9, 0x23, // Source Setting
    0x02, 0xB8, 0x09, // Source Setting
    0x01, 0x11, // Sleep out
    0x02, 0xff, 100, // delay 100
    0x02, 0xD0, 0x1F, // enable auto power down
    0x01, 0x39, // Low power mode
    0x01, 0x29, // display on
    0x00
};
void ST7302_init(int iSpeed, uint8_t u8DC, uint8_t u8CS, uint8_t u8RST)
{
	uint8_t *s, u8Count;

	u8DCPin = u8DC; u8CSPin = u8CS; u8RSTPin = u8RST;
	SPI_begin(iSpeed, 0);
	// reset LCD
	pinMode(u8DCPin, OUTPUT);
	pinMode(u8CSPin, OUTPUT);
	pinMode(u8RSTPin, OUTPUT);
	digitalWrite(u8RSTPin, 1);
    delay(200);
	digitalWrite(u8RSTPin, 0);
	delay(10);
	digitalWrite(u8RSTPin, 1);
    delay(200);

	// send init sequence
    s = (uint8_t*) st7302_init;
	u8Count = *s++;
    while (u8Count) {
    	if (s[0] == 0xff) { // delay
    		delay(s[1]);
    		s += 2;
    	} else {
    		ST7302_writeCMD(s[0]);
    		s++;
    		u8Count--;
    		if (u8Count) {
    			ST7302_writeDATA(s, u8Count);
    			s += u8Count;
    		}
    	}
    	u8Count = *s++;
    }
} /* ST7302_init() */

void ST7302_writeCMD(uint8_t u8CMD)
{
    digitalWrite(u8CSPin, 0);
	digitalWrite(u8DCPin, 0);
	SPI_write(&u8CMD, 1);
    digitalWrite(u8CSPin, 1);
}

void ST7302_writeDATA(uint8_t *pData, int iLen)
{
    digitalWrite(u8CSPin, 0);
	digitalWrite(u8DCPin, 1);
	SPI_write(pData, iLen);
    digitalWrite(u8CSPin, 1);
}
void ST7302_setPos(int x, int y)
{
uint8_t ucTemp[4];

    cursor_x = x; cursor_y = y;
    ST7302_writeCMD(0x2a); // Column set
    ucTemp[0] = 0x19 + (y/12); // start x (we treat it as y)
    ucTemp[1] = 0x27; // end x
    ST7302_writeDATA(ucTemp, 2);
    ST7302_writeCMD(0x2b); // Row set
    ucTemp[0] = (x/2); // start y (we treat it as x)
    ucTemp[1] = 0x80; // end y
    ST7302_writeDATA(ucTemp, 2);
    ST7302_writeCMD(0x2c); // memory write
} /* ST7302_setPos() */

void ST7302_fill(uint8_t u8Pattern)
{
  memset(u8Cache, u8Pattern, 33);
  for (int x=0; x<LCD_WIDTH; x+=2) {
	  ST7302_setPos(x, 0);
	  ST7302_writeDATA(u8Cache, 33); // work left to right 2 cols at a time
  }
  cursor_x = cursor_y = 0;
} /* ST7302_fill() */

//
// Draw a string of characters in a custom font
// A back buffer must be defined
//
void ST7302_print(const GFXfont *pFont, int x, int y, char *szMsg, uint8_t u8Color)
{
	int i, end_y, dx, dy, tx, ty, iBitOff;
	unsigned int c;
	uint8_t *s, *d, bits, ucCacheLine, ucFill=0, ucMask, uc;
	GFXfont font;
	GFXglyph glyph, *pGlyph;

	    if (x == -1)
	        x = cursor_x;
	    if (y == -1)
	        y = cursor_y;
	   // in case of running on Harvard architecture, get copy of data from FLASH
	   memcpy(&font, pFont, sizeof(font));
	   pGlyph = &glyph;

	   i = 0;
	   while (szMsg[i] && x < LCD_WIDTH)
	   {
	      c = szMsg[i++];
	      if (c < font.first || c > font.last) // undefined character
	         continue; // skip it
	      c -= font.first; // first char of font defined
	      memcpy(&glyph, &font.glyph[c], sizeof(glyph));
	      dx = x + pGlyph->xOffset; // offset from character UL to start drawing
	      dy = y + pGlyph->yOffset;
	      s = font.bitmap + pGlyph->bitmapOffset; // start of bitmap data
	      // Bitmap drawing loop. Image is MSB first and each pixel is packed next
	      // to the next (continuing on to the next character line)
	      iBitOff = 0; // bitmap offset (in bits)
	      bits = uc = 0; // bits left in this font byte
	      end_y = dy + pGlyph->height;
	      if (dy < 0) { // skip these lines
	          iBitOff += (pGlyph->width * (-dy));
	          dy = 0;
	      }
	      memset(u8Cache, ucFill, sizeof(u8Cache));
          ucMask = 1<<(dy & 7); // destination bit number for this starting point
          ucCacheLine = 0;
	      for (ty=dy; ty<end_y && ty < LCD_HEIGHT; ty++) {
	          d = &u8Cache[ucCacheLine * LCD_WIDTH]; // no backing ram; buffer 24 lines at a time
	         for (tx=0; tx<pGlyph->width; tx++) {
	            if (bits == 0) { // need to read more font data
	               uc = s[iBitOff>>3]; // get more font bitmap data
	               bits = 8 - (iBitOff & 7); // we might not be on a byte boundary
	               iBitOff += bits; // because of a clipped line
	               uc <<= (8-bits);
	            } // if we ran out of bits
	            if ((dx+tx) < LCD_WIDTH) { // foreground pixel
	                if (uc & 0x80) {
	                   if (u8Color == 1)
	                      d[tx] |= ucMask;
	                   else
	                      d[tx] &= ~ucMask;
	                } else {
	                    if (u8Color == 1)
	                       d[tx] &= ~ucMask;
	                    else
	                       d[tx] |= ucMask;
	                }
	            }
	            bits--; // next bit
	            uc <<= 1;
	         } // for x
	         if ((ucMask == 0x8 && ucCacheLine == 1) || ty == end_y-1) { // dump this line
	        	  uint8_t ucTemp[4];
	        	  int j, k;
	        	  uint8_t uc, ucSrc, *dp, *ppix;
	        	  for (j=0; j < pGlyph->width; j+=2)
	        	  { // convert to odd layout of LCD
	        		  ucSrc = 0x1; // top 8 pixels
	        		  uc = 0;
	        		  ppix = &u8Cache[j];
	        		  dp = ucTemp;
	        		  for (k=0; k<12; k++) { // capture 12 columns
	        			  uc <<= 2;
                    	  if (ppix[0] & ucSrc) uc |= 2;
                    	  if (ppix[1] & ucSrc) uc |= 1;
                    	  ucSrc <<= 1;
                    	  if ((k & 3) == 3) {
                    		  *dp++ = uc;
                    		  uc = 0;
                    		  if (ucSrc == 0) {
                    			  ucSrc = 0x1; // last 4 pixels
        	        		  	  ppix = &u8Cache[j+LCD_WIDTH];
                    		  }
                    	  }
                      }
	        		  ST7302_setPos(dx+j, ty-11);
	        		  ST7302_writeDATA(ucTemp, 3);
	        	  } // for j
	              memset(u8Cache, ucFill, sizeof(u8Cache));
	              ucMask = 1;
	              ucCacheLine = 0;
	          } else if (ucMask == 0x80) { // next byte row
		       	  ucMask = 1;
		       	  ucCacheLine = 1;
		      } else {
	        	  ucMask <<= 1;
	          }
	      } // for y
	      x += pGlyph->xAdvance; // width of this character
	   } // while drawing characters
	   cursor_x = x;
	   cursor_y = y;
} /* ST7302_print() */

