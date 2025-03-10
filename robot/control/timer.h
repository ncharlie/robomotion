#ifndef _CUSTOM_TIMER_H_
#define _CUSTOM_TIMER_H_
#include <avr/interrupt.h>
#include <avr/iomxx0_1.h>

class Timer {
   public:
    unsigned long timeStamp;
    Timer() : timeStamp(0) {
        TIMSK0 &= ~_BV(OCIE0A);  // Disable Timer/counter0 interrupt

        // Clear the Power Reduction Timer/Counter0 bit to enable Timer/Counter0
        PRR0 &= ~_BV(PRTIM0);

        // Set normal operation COM0x[1:0] = 0 0
        // WGM to Fast PWM (Mode 14, p145 Datasheet) WGM3[1:0] = 1 0
        TCCR0A = _BV(WGM01);

        // No force output FOC0A = 0, FOC0B = 0
        // WGM to CTC (Mode 2) WGM02 = 0
        // Prescale 64 CS0[2:0] = 0 1 1
        TCCR0B = _BV(CS01) | _BV(CS00);

        OCR0A = 249;
        TCNT0 = 0;  // Reset Timer

        TIMSK0 |= _BV(OCIE0A);  // Enable Timer/counter0 interrupt
    }
};

// unsigned long Timer::timeStamp = 0;

Timer timer;

ISR(TIMER0_COMPA_vect) {
    timer.timeStamp++;
}

unsigned long cMilli() {
    return timer.timeStamp;
}

static void __empty() {
    // Empty
}

void cDelay(unsigned long ms) {
    uint32_t start = timer.timeStamp;

    while ((timer.timeStamp - start) < ms) {
        __empty();
    }
}

#endif /* _CUSTOM_TIMER_H_ */