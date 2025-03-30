#ifndef _COMPASS_H_
#define _COMPASS_H_
#include <Wire.h>

#define I2C_ADDRESS 0x60

class Compass {
   public:
    Compass() {
        Wire.begin();  // conects I2C
    }

    int read() {
        Wire.beginTransmission(I2C_ADDRESS);  // starts communication with cmps03
        Wire.write(0x2);                      // Sends the register we wish to read
        Wire.endTransmission();

        Wire.requestFrom(I2C_ADDRESS, 2);  // requests high byte

        while (Wire.available() < 2);    // while there is a byte to receive
        uint8_t highByte = Wire.read();  // reads the byte as an integer
        uint8_t lowByte = Wire.read();
        return ((highByte << 8) + lowByte);
    }
};

Compass compass;

#endif /* _COMPASS_H_ */