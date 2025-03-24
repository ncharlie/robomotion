#ifndef _SPEED_CONTROL_H_
#define _SPEED_CONTROL_H_

#include <avr/interrupt.h>

class Speed {
   public:
    volatile unsigned long frSpd = 0;  // 64 * 12 = 768 per 1 round (314 mm)
    volatile unsigned long rrSpd = 0;
    volatile unsigned long frLast = 0;

    Speed() {
        // --------------- MONITOR -------------------
        EIMSK &= ~_BV(INT0);  // Disable INT0 interrupt
        DDRD &= ~_BV(DDD0);   // set port 21 to input
        PORTD |= _BV(PD0);    // use pull-up register

        EICRA = _BV(ISC01) | _BV(ISC00);  // set INT4 to detect falling edge
        EIMSK |= _BV(INT0);               // Enable INT0 interrupt
    }
};

ISR(INT0_vect) {
    unsigned long now = micros();
    unsigned long elapsed;
    if (now > rover.frLast)
        elapsed = now - rover.frLast;
    else
        elapsed = now + (ULONG_MAX - rover.frLast);

    if (elapsed > 1000000)
        rover.frSpd = 0;
    else
        rover.frSpd = 78125 / elapsed;

    rover.frLast = now;
}

#endif /* _SPEED_CONTROL_H_ */