#include <arduino.h>
#include <avr/io.h>

// #include "din.h"
#include "controller.h"
#include "dout.h"
#include "motion.h"
#include "timer.h"

int main() {
    SREG &= ~_BV(SREG_I);  // Disable global interrupt

    DigitalOutput led(DDRB, DDB7, PORTB, PB7);

    // controller
    auto co = Controller();

    SREG |= _BV(SREG_I);  // Enable global interrupt

    unsigned long now = 0;
    unsigned long secondTrigger = 0;  // last time the input pin was triggered

    auto spin = [&now, &rover, &led](void) {
        rover.setSpeed(MAX_SPEED);
        if (now < 2000) {
            led.On();
            rover.setDirection(CWSPIN);
        } else if (now < 4000) {
            led.On();
            rover.setDirection(CCWSPIN);
        } else if (now < 5500) {
            led.On();
            rover.setDirection(RIGHTTURN);
        } else if (now < 7000) {
            led.On();
            rover.setDirection(LEFTTURN);
        } else {
            led.Off();
            rover.setSpeed(0);
        }
    };

    auto moveLilBit = [&rover](void) {
        if (rover.frSpd < 1536) {
            rover.setDirection(FORWARD);
            rover.setSpeed(MAX_SPEED);
        } else if (rover.frSpd < 3072) {
            rover.setDirection(BACKWARD);
            rover.setSpeed(MAX_SPEED / 2);
        } else {
            rover.setSpeed(0);
        }
    };

    Serial.begin(9600);

    for (;;) {
        now = cMilli();
        if (now - secondTrigger >= 1000) {
            // run every 1 second
            // Serial.println(count);
            secondTrigger = now;
        }

        uint8_t buttonState = co.read();
        Serial.println(buttonState);

        bool ledOn = true;

        if (buttonState == 0) {
            ledOn = false;
            spin();
        } else if (buttonState == 1) {
            ledOn = false;
            moveLilBit();
        } else if (buttonState == 4) {  // forward
            rover.setDirection(FORWARD);
            rover.setSpeed(MAX_SPEED);
        } else if (buttonState == 5) {  // backward
            rover.setDirection(BACKWARD);
            rover.setSpeed(MAX_SPEED / 2);
        } else if (buttonState == 2) {  // rightward
            rover.setDirection(RIGHT);
            rover.setSpeed(MAX_SPEED / 4);
        } else if (buttonState == 3) {  // leftward
            rover.setDirection(LEFT);
            rover.setSpeed(MAX_SPEED / 4);
        } else if (buttonState == 6) {  // clockwise spin
            rover.setDirection(LEFT);
            rover.setSpeed(MAX_SPEED / 5);
        } else if (buttonState == 7) {  // counter clockwise spin
            rover.setDirection(LEFT);
            rover.setSpeed(MAX_SPEED / 5);
        } else {
            ledOn = false;
            rover.setSpeed(0);
        }

        ledOn ? led.On() : led.Off();
    }

    return 0;
}