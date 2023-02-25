#include "Arduino.h"
#include "neopixels.h"

uint8_t NEOPIXELS_BUFFER[NUMPIXELS][3] = {};
uint16_t neopixel_gpio_pin;
GPIO_TypeDef * neopixel_gpio_typedef;


void pixels_begin()
{
    pinMode(NEOPIXEL_PIN, OUTPUT);
    neopixel_gpio_pin = GPIO_Pin_0 << (NEOPIXEL_PIN & 0xf);
    switch (NEOPIXEL_PIN & 0xf0) {
    case 0xa0:
        neopixel_gpio_typedef = GPIOA;
        break;
    case 0xc0:
        neopixel_gpio_typedef = GPIOC;
        break;
    case 0xd0:
        neopixel_gpio_typedef = GPIOD;
        break;
    }
}

void pixels_SendBit(uint8_t bit)
{

    if (bit) {
    // For a logical 1, send high-high-high-low signal to the GPIO pin
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_SET);
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_SET);
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_SET);
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_RESET);
    } else {
    // For a logical 0, send high-low-low-low signal to the GPIO pin
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_SET);
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_RESET);
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_RESET);
        GPIO_WriteBit(neopixel_gpio_typedef, neopixel_gpio_pin, Bit_RESET);
    }

}

void pixels_SendColour(uint8_t red, uint8_t green, uint8_t blue)
{
    // Send the green component first (MSB)
    for (int i = 7; i >= 0; i--) {
        pixels_SendBit((green >> i) & 1);
    }
    // Send the red component next
    for (int i = 7; i >= 0; i--) {
        pixels_SendBit((red >> i) & 1);
    }
    // Send the blue component last (LSB)
    for (int i = 7; i >= 0; i--) {
        pixels_SendBit((blue >> i) & 1);
    }
}

void pixels_setPixelColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue){
    NEOPIXELS_BUFFER[pixel][0] = red;
    NEOPIXELS_BUFFER[pixel][1] = green;
    NEOPIXELS_BUFFER[pixel][2] = blue;
}

void pixels_clear(){
    for(int i= 0; i < NUMPIXELS; i++){
        NEOPIXELS_BUFFER[i][0] = 0;
        NEOPIXELS_BUFFER[i][1] = 0;
        NEOPIXELS_BUFFER[i][2] = 0;
    }
}

void pixels_fill(uint8_t red, uint8_t green, uint8_t blue){
    for(int i= 0; i < NUMPIXELS; i++){
        NEOPIXELS_BUFFER[i][0] = red;
        NEOPIXELS_BUFFER[i][1] = green;
        NEOPIXELS_BUFFER[i][2] = blue;
    }
}

//https://www.cnblogs.com/wahahahehehe/p/16853802.html
void ColorHSV(u_int32_t hue, u_int32_t sat, u_int32_t val, uint8_t rgb[3]) {
        hue %= 360; // h -> [0,360]
        uint8_t rgb_max = val * 2.55f;
        uint8_t rgb_min = rgb_max * (100 - sat) / 100.0f;

        uint32_t i = hue / 60;
        uint32_t diff = hue % 60;

        // RGB adjustment amount by hue
        uint8_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

        switch (i) {
        case 0:
            rgb[0] = rgb_max;
            rgb[1] = rgb_min + rgb_adj;
            rgb[2] = rgb_min;
            break;
        case 1:
            rgb[0] = rgb_max - rgb_adj;
            rgb[1] = rgb_max;
            rgb[2] = rgb_min;
            break;
        case 2:
            rgb[0] = rgb_min;
            rgb[1] = rgb_max;
            rgb[2] = rgb_min + rgb_adj;
            break;
        case 3:
            rgb[0] = rgb_min;
            rgb[1] = rgb_max - rgb_adj;
            rgb[2] = rgb_max;
            break;
        case 4:
            rgb[0] = rgb_min + rgb_adj;
            rgb[1] = rgb_min;
            rgb[2] = rgb_max;
            break;
        default:
            rgb[0] = rgb_max;
            rgb[1] = rgb_min;
            rgb[2] = rgb_max - rgb_adj;
            break;
        }
}

void pixels_show(){
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels_SendColour(NEOPIXELS_BUFFER[i][0], NEOPIXELS_BUFFER[i][1], NEOPIXELS_BUFFER[i][2]);

    }
}
