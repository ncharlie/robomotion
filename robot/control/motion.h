#ifndef _MOTION_CONTROL_H_
#define _MOTION_CONTROL_H_
#include <avr/interrupt.h>
#include <avr/io.h>
#include <binary.h>

#define MAX_SPEED 256

enum Direction {
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
   public:
    volatile unsigned long frSpd = 0;  // 64 * 12 = 768 per 1 round (314 mm)
    volatile unsigned long rrSpd = 0;

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
        // counter 3 for pin 2,3,5 <--- right
        // counter 4 for pin 6,7,8 <--- left
        DDRE |= _BV(DDE5) | _BV(DDE4);  // set speed out pins 2,3 to output
        DDRH |= _BV(DDH5) | _BV(DDH4);  // set speed out pins 7,8 to output

        // Clear the Power Reduction Timer/Counter3,4 bit to enable Timer/Counter3,4
        PRR1 &= ~_BV(PRTIM4) & ~_BV(PRTIM3);

        // Set Pin Low on compare match (Non-inverting mode) COM3x[1:0] = 1 0
        // WGM to Fast PWM (Mode 14, p145 Datasheet) WGM3[1:0] = 1 0
        TCCR3A = _BV(COM3B1) | _BV(COM3C1) | _BV(WGM31);
        TCCR4A = _BV(COM4B1) | _BV(COM3C1) | _BV(WGM41);

        // No input ICNC3 = 0, ICES3 = 0
        // WGM to Fast PWM (Mode 14) WGM3[3:2] = 1 1
        // No prescaling CS3[2:0] = 0 0 1
        TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS30);
        TCCR4B = _BV(WGM43) | _BV(WGM42) | _BV(CS40);

        ICR3 = ICR4 = MAX_SPEED;  // Set top value for 16-bit PWM
        OCR3B = OCR3C = 0;        // Reset the compare value(speed)
        OCR4B = OCR4C = 0;        // Reset the compare value(speed)
        TCNT3 = TCNT4 = 0;        // Reset Timer

        // --------------- MONITOR -------------------
        EIMSK &= ~_BV(INT0);  // Disable INT0 interrupt
        DDRD &= ~_BV(DDD0);   // set port 21 to input
        PORTD |= _BV(PD0);    // use pull-up register

        EICRA = _BV(ISC01) | _BV(ISC00);  // set INT4 to detect falling edge
        EIMSK |= _BV(INT0);               // Enable INT0 interrupt
    }

    void setDirection(Direction dir) {
        PORTC = dir;
    }

    void setSpeed(uint16_t speed) {
        OCR3B = OCR3C = OCR4B = OCR4C = speed;
    }
};

Motion rover;
ISR(INT0_vect) {
    rover.frSpd++;
}

#endif /* _MOTION_CONTROL_H_ */
