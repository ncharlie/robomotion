#include <arduino.h>
#include <avr/io.h>

#include "compass.h"
#include "controller.h"
#include "data.h"
#include "dout.h"
#include "motion.h"
#include "speed.h"

int main() {
    init();

    SREG &= ~_BV(SREG_I);  // Disable global interrupt

    DigitalOutput led(DDRB, DDB7, PORTB, PB7);

    // controller
    auto co = Controller();

    SREG |= _BV(SREG_I);  // Enable global interrupt

    unsigned long now = 0;
    unsigned long secondTrigger = 0;  // last time the input pin was triggered

    auto showMoves = [&rover](void) {
        rover.setPower(MAX_POWER / 2);
        rover.setDirection(FORWARD);
        delay(2000);
        rover.setDirection(BACKWARD);
        delay(2000);
        rover.setDirection(RIGHT);
        delay(2000);
        rover.setDirection(LEFT);
        delay(2000);
        rover.setDirection(RIGHTTURN);
        delay(1500);
        rover.setDirection(LEFTTURN);
        delay(1500);
        rover.setDirection(CWSPIN);
        delay(1500);
        rover.setDirection(CCWSPIN);
        delay(1500);
        rover.setDirection(STOP);
    };

    Serial.begin(9600);

    rover.setDirection(STOP);
    rover.setPower(0);
    delay(1000);
    // showMoves();

    for (;;) {
        uint8_t buttonState = co.read();

        now = millis();
        if (now - secondTrigger >= 1000) {
            // run every 1 second
            // Serial.println(count);
            secondTrigger = now;

            rover.frSpd.updateSpeed();
            Serial.print(buttonState);
            Serial.print(" ");
            Serial.print(compass.read());
            Serial.print(" ");
            Serial.println(rover.frSpd.getSpeed());
        }

        bool ledOn = true;

        if (buttonState == 0) {
            ledOn = false;
            rover.setDirection(STOP);
            rover.setPower(0);
        } else if (buttonState == 1) {
            ledOn = false;
            rover.setPower(0);
        } else if (buttonState == 4) {  // forward
            rover.setDirection(FORWARD);
            rover.setPower(MAX_POWER);
        } else if (buttonState == 5) {  // backward
            rover.setDirection(BACKWARD);
            rover.setPower(MAX_POWER / 2);
        } else if (buttonState == 2) {  // rightward
            rover.setDirection(RIGHT);
            rover.setPower(MAX_POWER / 4);
        } else if (buttonState == 3) {  // leftward
            rover.setDirection(LEFT);
            rover.setPower(MAX_POWER / 4);
        } else if (buttonState == 6) {  // clockwise spin
            rover.setDirection(CWSPIN);
            rover.setPower(MAX_POWER / 5);
        } else if (buttonState == 7) {  // counter clockwise spin
            rover.setDirection(CCWSPIN);
            rover.setPower(MAX_POWER / 5);
        } else {
            ledOn = false;
            rover.setPower(0);
        }

        ledOn ? led.On() : led.Off();
    }

    return 0;
}