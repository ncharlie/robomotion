#ifndef _MOTION_CONTROL_H_
#define _MOTION_CONTROL_H_
#include <avr/interrupt.h>
#include <avr/io.h>
#include <binary.h>
#include <limits.h>

#include "speed.h"

enum Direction {
    STOP = B00000000,
    FORWARD = 0B01100110,
    RIGHT = B10100101,
    BACKWARD = ~FORWARD,
    LEFT = ~RIGHT,
    CWSPIN = B10010110,
    CCWSPIN = ~CWSPIN,
    RIGHTTURN = B00100100,
    LEFTTURN = B01000010
};

class Motion {
   private:
    int targetSpeed;

   public:
    Speed frSpd;
    Speed rrSpd;
    Speed flSpd;
    Speed rlSpd;

   public:
    Motion() {
        // -------------- DIRECTION ------------------
        // 30 (PC7), 31 (PC6) <--- front-right
        // 32 (PC5), 33 (PC4) <--- rear-right
        // 34 (PC3), 35 (PC2) <--- front-left
        // 36 (PC1), 37 (PC0) <--- rear-left
        DDRC = B11111111;   // set pins 30-37 to output
        PORTC = B00000000;  // init direction to 0

        // ---------------- SPEED --------------------
        // counter 1 for pin 11,12,13 <--- right
        // counter 3 for pin 2,3,5 <--- right
        // counter 4 for pin 6,7,8 <--- left
        DDRB |= _BV(DDB6) | _BV(DDB5);  // set speed out pins 11,12 to output
        // DDRE |= _BV(DDE5) | _BV(DDE4);  // set speed out pins 2,3 to output
        DDRH |= _BV(DDH5) | _BV(DDH4);  // set speed out pins 7,8 to output

        // Clear the Power Reduction Timer/Counter3,4 bit to enable Timer/Counter3,4
        // PRR1 &= ~_BV(PRTIM4) & ~_BV(PRTIM3);
        // Clear the Power Reduction Timer/Counter1,4 bit to enable Timer/Counter1,4
        PRR0 &= ~_BV(PRTIM1);
        PRR1 &= ~_BV(PRTIM4);

        // Set Pin Low on compare match (Non-inverting mode) COM3x[1:0] = 1 0
        // WGM to Fast PWM (Mode 14, p145 Datasheet) WGM3[1:0] = 1 0
        TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
        // TCCR3A = _BV(COM3B1) | _BV(COM3C1) | _BV(WGM31);
        TCCR4A = _BV(COM4B1) | _BV(COM3C1) | _BV(WGM41);

        // No input ICNC3 = 0, ICES3 = 0
        // WGM to Fast PWM (Mode 14) WGM3[3:2] = 1 1
        // No prescaling CS3[2:0] = 0 0 1
        TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
        // TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS30);
        TCCR4B = _BV(WGM43) | _BV(WGM42) | _BV(CS40);

        ICR1 = ICR4 = MAX_POWER;  // Set top value for 16-bit PWM
        OCR1A = OCR1B = 0;        // Reset the compare value(speed)
        OCR4B = OCR4C = 0;        // Reset the compare value(speed)
        TCNT1 = TCNT4 = 0;        // Reset Timer

        // --------------- MONITOR -------------------
        EIMSK &= ~_BV(INT5) & ~_BV(INT4) & ~_BV(INT3) & ~_BV(INT2);  // Disable INT2-5 interrupt
        DDRD &= ~_BV(DDD3) & ~_BV(DDD2);                             // set port 18-19 to input
        DDRE &= ~_BV(DDE5) & ~_BV(DDE4);                             // set port 2-3 to input
        PORTD |= _BV(PD3) | _BV(PD2);                                // use pull-up register
        PORTE |= _BV(PE5) | _BV(PE4);                                // use pull-up register

        EICRA = _BV(ISC31) | _BV(ISC30) | _BV(ISC21) | _BV(ISC20);  // set INT2-3 to detect rising edge
        EICRB = _BV(ISC51) | _BV(ISC50) | _BV(ISC41) | _BV(ISC40);  // set INT4-5 to detect rising edge
        EIMSK |= _BV(INT5) | _BV(INT4) | _BV(INT3) | _BV(INT2);     // Enable INT2-5 interrupt

        targetSpeed = 0;
    }

    void setDirection(Direction dir) {
        PORTC = dir;
    }

    void setPower(uint16_t powerLevel) {
        OCR1A = OCR1B = OCR4B = OCR4C = powerLevel;
    }

    void setSpeed(int speed) {
        targetSpeed = speed;
    }

    void controlSpeed() {
        frSpd.updateSpeed();
        // rrSpd.updateSpeed();
        // flSpd.updateSpeed();
        // rlSpd.updateSpeed();

        OCR1A = (unsigned long)frSpd.checkSpeed(targetSpeed);
        // OCR1B = (unsigned long) rrSpd.checkSpeed(targetSpeed);
        // OCR4C = (unsigned long) flSpd.checkSpeed(targetSpeed);
        // OCR4B = (unsigned long) rlSpd.checkSpeed(targetSpeed);
    }
};

Motion rover;

ISR(INT5_vect) {
    rover.frSpd.countSinceLast++;
    Serial.print(".");
}
ISR(INT4_vect) {
    rover.rrSpd.countSinceLast++;
    Serial.print(".");
}
ISR(INT3_vect) {
    rover.flSpd.countSinceLast++;
    Serial.print(".");
}
ISR(INT2_vect) {
    rover.rlSpd.countSinceLast++;
    Serial.print(".");
}

#endif /* _MOTION_CONTROL_H_ */
