/*
 * Arduino.h
 *
 *  Created on: Jan 12, 2023
 *      Author: larry
 */

#ifndef USER_ARDUINO_H_
#define USER_ARDUINO_H_

#include "debug.h"

int main();
void setup();
void loop();


#define A1 0xA1
#define A2 0xA2

#define C0 0xC0
#define C1 0xC1
#define C2 0xC2
#define C3 0xC3
#define C4 0xC4
#define C5 0xC5
#define C6 0xC6
#define C7 0xC7

#define D0 0xD0
#define D1 0xD1
#define D2 0xD2
#define D3 0xD3
#define D4 0xD4
#define D5 0xD5
#define D6 0xD6
#define D7 0xD7

#define LED_BUILTIN D4
#define BUTTON_BUILTIN D7

#define HIGH 1
#define LOW 0

// GPIO pin states
enum {
	OUTPUT = 0,
	INPUT,
	INPUT_PULLUP,
	INPUT_PULLDOWN
};

#define PROGMEM
#define memcpy_P memcpy
#define pgm_read_byte(s) *(uint8_t *)s
#define pgm_read_word(s) *(uint16_t *)s

void Serial_begin(uint32_t baudrate);
void Serial_println(const char *serialmessage);

// Wrapper methods
void delay(int i);
//
// Digital pin functions use a numbering scheme to make it easier to map the
// pin number to a port name and number
// The GPIO ports A-D become the most significant nibble of the pin number
// for example, to use Port C pin 7 (C7), use the pin number 0xC7
//
void pinMode(uint8_t u8Pin, int iMode);
uint8_t digitalRead(uint8_t u8Pin);
void digitalWrite(uint8_t u8Pin, uint8_t u8Value);


// Random stuff
void Standby82ms(uint8_t iTicks);
void breatheLED(uint8_t u8Pin, int iPeriod);


#endif /* USER_ARDUINO_H_ */
