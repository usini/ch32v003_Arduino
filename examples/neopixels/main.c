#include "Arduino.h"
#include "neopixels.h"


/*
 To setup the number of leds you need to change Peripheral/neopixels.h
 By default
 NEOPIXEL_PIN = D3
 NUMPIXELS = 16

*/

void setup() {
    Serial_begin(115200);
    pixels_begin();
    printf("START\r\n");
    pixels_clear();
}

void loop() {
    pixels_setPixelColor(0,64,0,0);
    pixels_show();
    delay(1000);

    pixels_setPixelColor(0, 0, 0, 0);
    pixels_show();
    delay(1000);
}
