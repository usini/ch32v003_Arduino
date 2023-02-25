// Simple demonstration on using an input device to trigger changes on your
// NeoPixels. Wire a momentary push button to connect from ground to a
// digital IO pin. When the button is pressed it will change to a new pixel
// animation. Initial state has all pixels off -- press the button once to
// start the first animation. As written, the button does not interrupt an
// animation in-progress, it works only when idle.

#include "Arduino.h"
#include "neopixels.h"

// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
#define BUTTON_PIN   BUTTON_BUILTIN

void colorWipe(uint8_t red, uint8_t green, uint8_t blue, int wait);
void theaterChase(uint8_t red, uint8_t green, uint8_t blue, int wait);
void theaterChaseRainbow(int wait);
void rainbow(int wait);

u_int8_t oldState = HIGH;
int     mode     = 0;    // Currently-active animation mode, 0-9

void setup() {
  Serial_begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pixels_begin(); // Initialize NeoPixel strip object (REQUIRED)
  pixels_show();  // Initialize all pixels to 'off'
}

void loop() {
    rainbow(50);
  /*
  // Get current button state.
   u_int8_t newState = digitalRead(BUTTON_PIN);

  // Check if state changed from high to low (button press).
  if((newState == LOW) && (oldState == HIGH)) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if(newState == LOW) {      // Yes, still low
      printf("MODE:%d\r\n", mode);

      if(++mode > 8) mode = 0; // Advance to next mode, wrap around after #8
      switch(mode) {           // Start the new animation...
        case 0:
          colorWipe(0,0,0,50);    // Black/off
          break;
        case 1:
          colorWipe(255,0,0,50);    // Red
          break;
        case 2:
          colorWipe(0,255,0,50);    // Green
          break;
        case 3:
          colorWipe(0,0,255,50);    // Blue
          break;
        case 4:
          theaterChase(127, 127, 127, 50); // White
          break;
        case 5:
          theaterChase(127,   0,   0, 50); // Red
          break;
        case 6:
          theaterChase(0,   0, 127, 50); // Blue
          break;
        case 7:
          rainbow(10);
          break;
        case 8:
          theaterChaseRainbow(50);
          break;
      }
    }
  }

  // Set the last-read button state to the old state.
  oldState = newState;
  */
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint8_t red, uint8_t green, uint8_t blue, int wait) {
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel in strip...
    pixels_setPixelColor(i, red, green, blue);         //  Set pixel's color (in RAM)
    pixels_show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint8_t red, uint8_t green, uint8_t blue, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      pixels_fill(0,0,0);         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<NUMPIXELS; c += 3) {
        pixels_setPixelColor(c, red, green, blue); // Set pixel 'c' to value 'color'
      }
      pixels_show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(u_int32_t hue = 0; hue <= 360; hue ++) {
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel in strip...

      uint8_t rgb[3];
      ColorHSV(hue, 255, 255, rgb);
      pixels_setPixelColor(i, rgb[0], rgb[1], rgb[2]);
    }
    pixels_show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      pixels_clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<NUMPIXELS; c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (NUMPIXELS steps):
        int      hue   = firstPixelHue + c * 65536L / NUMPIXELS;
        uint8_t rgb[3];
        ColorHSV(hue, 255,255, rgb);
        pixels_setPixelColor(c, rgb[0], rgb[1], rgb[2]); // Set pixel 'c' to value 'color'
      }
      pixels_show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
