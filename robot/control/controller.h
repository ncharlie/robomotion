#ifndef _USER_CONTROL_H_
#define _USER_CONTROL_H_
#include <avr/io.h>

#include "din.h"

#define DEBOUNCE_DELAY 200UL

class Controller {
   private:
    uint8_t value = 0;
    uint8_t lastReading = 0;            // previous reading of input pins
    unsigned long lastReadingTime = 0;  // last time the input pin was triggered
   public:
    Controller() {
        auto pin42 = DigitalInput(DDRL, DDL7, PORTL, PL7, PINL, PINL7);  // set pin 42 to input
        auto pin43 = DigitalInput(DDRL, DDL6, PORTL, PL6, PINL, PINL6);  // set pin 43 to input
        auto pin44 = DigitalInput(DDRL, DDL5, PORTL, PL5, PINL, PINL5);  // set pin 44 to input
    }

    uint8_t read() {
        uint8_t reading = ~(PINL & (_BV(PINL7) | _BV(PINL6) | _BV(PINL5)));
        reading = reading >> 5;

        if (reading != lastReading) {
            lastReadingTime = millis();  // keep timestamp
        }
        lastReading = reading;

        if (millis() - lastReadingTime > DEBOUNCE_DELAY) {
            value = reading;  // most likely not a debounce,
        }
        return value;
    }
};

#endif /* _USER_CONTROL_H_ */