#include <arduino.h>
#include <avr/io.h>

#include "compass.h"
#include "controller.h"
#include "dout.h"
#include "motion.h"
#include "speed.h"
// #include "data.h"

int main() {
    init();

    SREG &= ~_BV(SREG_I);  // Disable global interrupt

    DigitalOutput led(DDRB, DDB7, PORTB, PB7);

    SREG |= _BV(SREG_I);  // Enable global interrupt

    unsigned long now = 0;
    unsigned long secondTrigger = 0;  // last time the input pin was triggered
    unsigned long fastTrigger = 0;
    unsigned long commandTrigger = 0;

    Serial.begin(9600);
    Serial3.begin(115200);

    rover.setDirection(STOP);
    rover.setSpeed(0);
    delay(1000);

    for (;;) {
        now = millis();
        if (now - fastTrigger >= 200) {  // run every 200 ms
            rover.controlSpeed();
            compass.update();
            fastTrigger = millis();
        }

        if (now - secondTrigger >= 1500) {  // run every 1 second
            char log[100];
            sprintf(log, "Heading: %d\tFront: %lu-%lu\n\t\tBack : %lu-%lu\n", compass.read(), rover.flSpd.getSpeed(), rover.frSpd.getSpeed(), rover.rlSpd.getSpeed(), rover.rrSpd.getSpeed());
            Serial.print(log);

            uint16_t fl = OCR4B;
            uint16_t fr = OCR1A;
            uint16_t rl = OCR4C;
            uint16_t rr = OCR1B;

            sprintf(log, "Speed regis\tFront: %hu-%hu\n\t\tBack : %hu-%hu\n", fl, fr, rl, rr);
            Serial.print(log);

            char message[16];
            sprintf(message, "%lu,%d,%d\0", rover.frSpd.getSpeed(), compass.read(), false);
            Serial3.print(message);

            secondTrigger = millis();
        }

        bool ledOn = true;

        if (Serial3.available()) {
            String command = Serial3.readStringUntil('\n');
            command.trim();
            command.toUpperCase();
            Serial.print("Command received: ");
            Serial.println(command);
            int speed = 3000;

            commandTrigger = millis();
            if (command.equals("ST")) {
                rover.setDirection(STOP);
                speed = 0;
            } else if (command.equals("FW")) {
                rover.setDirection(FORWARD);
            } else if (command.equals("BW")) {
                rover.setDirection(BACKWARD);
            } else if (command.equals("RW")) {
                rover.setDirection(RIGHT);
            } else if (command.equals("RF")) {
                rover.setDirection(RIGHTFW);
            } else if (command.equals("RB")) {
                rover.setDirection(RIGHTBW);
            } else if (command.equals("RT")) {
                rover.setDirection(RIGHTTURN);
                commandTrigger -= 500;
            } else if (command.equals("LW")) {
                rover.setDirection(LEFT);
            } else if (command.equals("LF")) {
                rover.setDirection(LEFTFW);
            } else if (command.equals("LB")) {
                rover.setDirection(LEFTBW);
            } else if (command.equals("LT")) {
                rover.setDirection(LEFTTURN);
                commandTrigger -= 500;
            } else {
                rover.setDirection(STOP);
                speed = 0;
            }

            rover.setSpeed(speed);
        } else if (now - commandTrigger >= 10000) {
            rover.setDirection(STOP);
            rover.setSpeed(0);
        }

        ledOn ? led.On() : led.Off();
    }

    return 0;
}