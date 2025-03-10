#ifndef _DIGITAL_IN_H_
#define _DIGITAL_IN_H_

class DigitalInput {
   private:
    volatile uint8_t& pin;
    uint8_t pinLoc;

   public:
    DigitalInput(volatile uint8_t& ddr, uint8_t ddrLoc, volatile uint8_t& port, uint8_t portLoc, volatile uint8_t& pin, uint8_t pinLoc) : pin(pin), pinLoc(pinLoc) {
        ddr &= ~_BV(ddrLoc);   // set port to input
        port |= _BV(portLoc);  // use pull-up register
    }

    uint8_t Read() {
        // read 1 from pull-up if not connected, flip to 0
        // read 0 if connected to ground, flip to 1
        return !(pin & _BV(pinLoc));  // return 1 or 0
    }
};

#endif /* _DIGITAL_IN_H_ */