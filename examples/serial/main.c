#include "Arduino.h"

/*
 * Serial implementation
 * Remember you can use the debug, add a breakpoint (double-click on the left before the line number)
 * In expressions, add your variable to see it
 */

void setup() {
    Serial_begin(115200);
    // Use printf instead of Serial.println();

    // You need to add \r\n if you want to do a println
    printf("\r\n");

    // Text (Don't forget \r\n to make a new line)
    printf("Hello World\r\n");

    // Integer
    int variable_integer = 10;
    printf("Variable = %d\r\n", variable_integer);

    // Float
    // I didn't manage to display float :-(
    // https://forum.arduino.cc/t/no-sprintf-float-formatting-come-back-five-year/331790/4

    // String
    const char* variable_string = "This is a string";
    printf("Variable = %s\r\n", variable_string);
}


void loop() {

}
