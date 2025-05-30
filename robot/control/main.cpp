#include <arduino.h>
#include <avr/io.h>

#include "compass.h"
#include "dout.h"
#include "motion.h"
#include "speed.h"
#include "ultrasonic.h"

volatile int targetHeading = -1;

#define HEADING_THRESHOLD 25
#define DISTANCE_THRESHOLD 30

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
    unsigned long lastUpdate10ms = 0;  // last time the input pin was triggered
    unsigned long lastUpdate50ms = 0;
    unsigned long lastUpdate100ms = 0;   // last time the input pin was triggered
    unsigned long lastUpdate1500ms = 0;  // last time the input pin was triggered
    unsigned long commandTrigger = 0;

    int bearingAdjustPhase = 0;
    unsigned long lastBearingAdjust = 0;
    unsigned long lastBearingWait = 0;
    unsigned long bearingAdjustDelay = 0;

    Serial.begin(9600);
    Serial.flush();
    Serial3.begin(115200);

    rover.setDirection(STOP);
    rover.setSpeed(2);
    delay(1000);
    int dist[4] = {0, 0, 0, 0};
    int uPos = 0;
    for (;;) {
        int distance = ultrasonic.read(uPos);
        // int y0 = ultrasonic.read(0);
        // int y1 = ultrasonic.read(1);
        // int x0 = ultrasonic.read(2);
        // int x1 = ultrasonic.read(3);
        // char uLog[100];
        // sprintf(uLog, "Front: %d\tBack: %d\tLeft: %d\tRight: %d\n", y0, y1, x0, x1);
        // Serial.print(uLog);
        dist[uPos] = distance;
        // Serial.println(dist[0]);

        if ( 0 <= distance && distance <= DISTANCE_THRESHOLD ) rover.setDirection(STOP);
        // if ( 0 <= y0 && y0 <= DISTANCE_THRESHOLD ) rover.setDirection(STOP);
        // if ( 0 <= y1 && y1 <= DISTANCE_THRESHOLD ) rover.setDirection(STOP);
        // if ( 0 <= x0 && x0 <= DISTANCE_THRESHOLD ) rover.setDirection(STOP);
        // if ( 0 <= x1 && x1 <= DISTANCE_THRESHOLD ) rover.setDirection(STOP);

        uPos = (uPos + 1)%4;
        now = millis();
        if (now - lastUpdate10ms >= 20) {  // run every 10 ms
            compass.update();
            lastUpdate10ms = millis();
        }

        if (now - lastUpdate100ms >= 100) {  // run every 100 ms
            rover.controlSpeed();
            ultrasonic.measure();
            lastUpdate100ms = millis();
        }

        if (now - lastUpdate1500ms >= 1500) {  // run every 1.5 second
            // char log[100];
            // sprintf(log, "Heading: %d\tFront: %lu-%lu\tBack : %lu-%lu\t", compass.read(), rover.flSpd.getSpeed(), rover.frSpd.getSpeed(), rover.rlSpd.getSpeed(), rover.rrSpd.getSpeed());
            // Serial.print(log);

            uint16_t fl = OCR4B;
            uint16_t fr = OCR1A;
            uint16_t rl = OCR4C;
            uint16_t rr = OCR1B;

            // sprintf(log, "Speed regis\tFront: %hu-%hu\t\tBack : %hu-%hu\n", fl, fr, rl, rr);
            // Serial.print(log);
            // sprintf(log, "Speed regis\tFront: %hu-%hu\tBack : %hu-%hu\n", OCR4B, OCR1A, OCR4C, OCR1B);
            // Serial.print(log);

            int y0 = ultrasonic.read(0);
            int y1 = ultrasonic.read(1);
            int x0 = ultrasonic.read(2);
            int x1 = ultrasonic.read(3);
            
            char message[32];
            sprintf(message, "%lu,%d,", rover.frSpd.getSpeed(), compass.read(), dist[0], dist[1], dist[2], dist[3]);
            // sprintf(message, "{\"r\":\"r1\",\"x\":%d,\"y\":%d,\"z\":0,\"s\":%lu,\"h\":%d}",
            // x, y, rover.frSpd.getSpeed(), compass.read());
            Serial.println(message);
            Serial3.println(message);
            
            // ultrasonic.read(0);
            // sprintf(log, "Front: %d\n", ultrasonic.read(0));
            // sprintf(log, "Front: %d\tBack: %d\tLeft: %d\tRight: %d\n", y0, y1, x0, x1);
            // Serial.print(log);

            lastUpdate1500ms = millis();
        }

        bool ledOn = true;

        // Serial.print("targetHeading: ");
        // Serial.println(targetHeading);
        if (targetHeading != -1) {
            if (bearingAdjustPhase == 0) {  // Initial phase, not moving
                int current = compass.read();
                if (current >= 0) {
                    int diff = targetHeading - current;
                    if (diff > 1800)
                        diff -= 3600;
                    else if (diff < -1800)
                        diff += 3600;

                    int aDiff = abs(diff);

                    rover.setSpeed(2);
                    if (aDiff < HEADING_THRESHOLD) {
                        rover.setDirection(STOP);
                        Serial.println("-----SUCCESS----");
                        targetHeading = -1;
                    } else if (diff > 0) {
                        rover.setDirection(RIGHTTURN);
                        Serial.print("-----MOVE RIGHT---- for:");
                    } else {
                        rover.setDirection(LEFTTURN);
                        Serial.print("-----MOVE LEFT---- for:");
                    }

                    bearingAdjustDelay = (5 * aDiff) / 6;
                    lastBearingAdjust = millis();
                    bearingAdjustPhase = 1;
                    Serial.println(bearingAdjustDelay);
                }
            } else if (bearingAdjustPhase == 1) {  // Adjusting phase
                if (millis() - lastBearingAdjust >= bearingAdjustDelay) {
                    rover.setDirection(STOP);
                    Serial.println("-----STOP----");
                    lastBearingWait = millis();
                    bearingAdjustPhase = 2;
                }
            } else if (bearingAdjustPhase == 2) {  // Delaying phase
                if (millis() - lastBearingWait >= 500) {
                    bearingAdjustPhase = 0;
                    Serial.println("-----DONE----");
                }
            }
        }

        if (Serial3.available()) {
            // if (Serial.available()) {
            String command = Serial3.readStringUntil('\n');
            // String command = Serial.readString();
            command.trim();
            command.toUpperCase();
            Serial.print("Command received: ");
            Serial.println(command);

            commandTrigger = millis();
            if (command.startsWith("H")) {
                targetHeading = command.substring(1).toInt();
                targetHeading *= 10;
                Serial.print("Target heading set: ");
                Serial.println(targetHeading);
            } else if (command.equals("ST")) {
                rover.setDirection(STOP);
                targetHeading = -1;
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

        Serial.flush();
        ledOn ? led.On() : led.Off();
    }

    return 0;
}
