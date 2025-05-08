#ifndef _MOTION_CONTROL_H_
#define _MOTION_CONTROL_H_
#include <avr/interrupt.h>
#include <avr/io.h>
#include <binary.h>

#include "speed.h"

enum Direction {
    STOP = B00000000,       // ·
    FORWARD = 0B01100110,   // ↑
    BACKWARD = ~FORWARD,    // ↓
    RIGHT = B10100101,      // →
    RIGHTFW = B00100100,    // ↗
    RIGHTBW = B10000001,    // ↘
    RIGHTTURN = B10010110,  // ↻ (Rotate clockwise)
    LEFT = ~RIGHT,          // ←
    LEFTFW = B01000010,     // ↖
    LEFTBW = B00011000,     // ↙
    LEFTTURN = ~RIGHTTURN   // ↺ (Rotate counterclockwise)
};

int spdLevel[7] = {0, 2800, 3800, 4800, 5800, 6800, 10000};
int frBase[7] = {0, 91, 120, 160, 233, 410, MAX_POWER};
int rrBase[7] = {0, 107, 138, 184, 258, 505, MAX_POWER};
int flBase[7] = {0, 164, 212, 315, 432, 768, MAX_POWER};
int rlBase[7] = {0, 75, 104, 158, 248, 590, MAX_POWER};

class Motion {
   private:
    int targetSpeed;
    int currentDirection;

   public:
    Speed frSpd;
    Speed rrSpd;
    Speed flSpd;
    Speed rlSpd;

   public:
    Motion() : targetSpeed(0), 
    frSpd(frBase, 0.01, 0.004, 0.004), 
    rrSpd(rrBase, 0.01, 0.004, 0.004), 
    flSpd(flBase, 0.01, 0.004, 0.004), 
    rlSpd(rlBase, 0.01, 0.004, 0.004) {
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
    }

    void setDirection(Direction dir) {
        if (dir == currentDirection) return;

        PORTC = dir;
        currentDirection = dir;
        frSpd.reset();
        rrSpd.reset();
        flSpd.reset();
        rlSpd.reset();
    }

    // void setPower(uint16_t powerLevel) {
    //     OCR1A = OCR1B = OCR4B = OCR4C = powerLevel;
    // }

    void setSpeed(int level) {
        targetSpeed = spdLevel[level];
        // Serial.print("Setting target speed to ");
        // Serial.println(targetSpeed);

        frSpd.reset();
        rrSpd.reset();
        flSpd.reset();
        rlSpd.reset();

        // OCR1A = (unsigned long)frBase[level];
        // OCR1B = (unsigned long)rrBase[level];
        // OCR4B = (unsigned long)flBase[level];
        // OCR4C = (unsigned long)rlBase[level];
    }

    void controlSpeed() {
        frSpd.updateSpeed(0);
        rrSpd.updateSpeed(1);
        flSpd.updateSpeed(0);
        rlSpd.updateSpeed(0);

        if (currentDirection && B11000000) OCR1A = (unsigned long)frSpd.checkSpeed(targetSpeed);
        if (currentDirection && B00110000) OCR1B = (unsigned long)rrSpd.checkSpeed(targetSpeed);
        if (currentDirection && B00001100) OCR4B = (unsigned long)flSpd.checkSpeed(targetSpeed);
        if (currentDirection && B11000011) OCR4C = (unsigned long)rlSpd.checkSpeed(targetSpeed);
    }
};

Motion rover;

ISR(INT5_vect) { rover.frSpd.countSinceLast++; }
ISR(INT4_vect) { rover.rrSpd.countSinceLast++; }
ISR(INT3_vect) { rover.flSpd.countSinceLast++; }
ISR(INT2_vect) { rover.rlSpd.countSinceLast++; }

#endif /* _MOTION_CONTROL_H_ */
