#include <arduino.h>
#include <avr/io.h>

#include "compass.h"
#include "dout.h"
#include "motion.h"
#include "speed.h"

int main() {
    init();

    SREG &= ~_BV(SREG_I);  // Disable global interrupt

    DigitalOutput led(DDRB, DDB7, PORTB, PB7);
    DDRH |= _BV(DDH5) | _BV(DDH4);
    PRR1 &= ~_BV(PRTIM4);
    TCCR4A = _BV(COM4B1) | _BV(COM3C1) | _BV(WGM41);
    TCCR4B = _BV(WGM43) | _BV(WGM42) | _BV(CS40);
    ICR1 = ICR4 = MAX_POWER;
    OCR4B = OCR4C = 0;
    TCNT4 = 0;

    SREG |= _BV(SREG_I);  // Enable global interrupt

    unsigned long now = 0;
    unsigned long lastUpdate20ms = 0;  // last time the input pin was triggered
    unsigned long lastUpdate200ms = 0;
    unsigned long lastUpdate1500ms = 0;  // last time the input pin was triggered
    unsigned long commandTrigger = 0;

    Serial.begin(9600);
    Serial3.begin(115200);

    rover.setDirection(STOP);
    rover.setSpeed(2);
    delay(1000);

    for (;;) {
        now = millis();
        if (now - lastUpdate20ms >= 100) {  // run every 100 ms
            rover.controlSpeed();
            lastUpdate20ms = millis();
        }

        if (now - lastUpdate200ms >= 200) {  // run every 200 ms
            compass.update();
            lastUpdate200ms = millis();
        }

        if (now - lastUpdate1500ms >= 1500) {  // run every 1 second
            char log[100];
            sprintf(log, "Heading: %d\tFront: %lu-%lu\tBack : %lu-%lu", compass.read(), rover.flSpd.getSpeed(), rover.frSpd.getSpeed(), rover.rlSpd.getSpeed(), rover.rrSpd.getSpeed());
            Serial.print(log);

            uint16_t fl = OCR4B;
            uint16_t fr = OCR1A;
            uint16_t rl = OCR4C;
            uint16_t rr = OCR1B;

            // sprintf(log, "Speed regis\tFront: %hu-%hu\n\t\tBack : %hu-%hu\n", fl, fr, rl, rr);
            sprintf(log, "Speed regis\tFront: %hu-%hu\tBack : %hu-%hu\n", OCR4B, OCR1A, OCR4C, OCR1B);
            Serial.print(log);

            char message[16];
            sprintf(message, "%lu,%d,%d\0", rover.frSpd.getSpeed(), compass.read(), false);
            Serial3.print(message);

            lastUpdate1500ms = millis();
        }

        bool ledOn = true;

        if (Serial3.available()) {
            // if (Serial.available()) {
            String command = Serial3.readStringUntil('\n');
            // String command = Serial.readString();
            command.trim();
            command.toUpperCase();
            Serial.print("Command received: ");
            Serial.println(command);

            commandTrigger = millis();
            if (command.equals("ST")) {
                rover.setDirection(STOP);
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
            } else if (command.equals("LW")) {
                rover.setDirection(LEFT);
            } else if (command.equals("LF")) {
                rover.setDirection(LEFTFW);
            } else if (command.equals("LB")) {
                rover.setDirection(LEFTBW);
            } else if (command.equals("LT")) {
                rover.setDirection(LEFTTURN);
            } else if (command.equals("S0")) {
                rover.setSpeed(0);
            } else if (command.equals("S1")) {
                rover.setSpeed(1);
            } else if (command.equals("S2")) {
                rover.setSpeed(2);
            } else if (command.equals("S3")) {
                rover.setSpeed(3);
            } else if (command.equals("S4")) {
                rover.setSpeed(4);
            } else if (command.equals("S5")) {
                rover.setSpeed(5);
            } else if (command.equals("S6")) {
                rover.setSpeed(6);
            } else {
                rover.setDirection(STOP);
            }
        } else if (now - commandTrigger >= 10000) {
            rover.setDirection(STOP);
        }

        ledOn ? led.On() : led.Off();
    }

    return 0;
}