#ifndef _DIGITAL_OUT_H_
#define _DIGITAL_OUT_H_

class DigitalOutput {
   private:
    volatile uint8_t& port;
    uint8_t portLoc;

   public:
    DigitalOutput(volatile uint8_t& ddr, uint8_t ddr_loc, volatile uint8_t& port, uint8_t portLoc) : port(port), portLoc(portLoc) {
        ddr |= _BV(ddr_loc);  // set port output
    }

    void On() {
        port |= _BV(portLoc);
    }

    void Off() {
        port &= ~_BV(portLoc);
    }

    void Toggle() {
        port ^= _BV(portLoc);
    }
};

#endif /* _DIGITAL_OUT_H_ */