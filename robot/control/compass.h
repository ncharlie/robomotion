#ifndef _COMPASS_H_
#define _COMPASS_H_
#include <Wire.h>

#define I2C_ADDRESS 0x60

#define COMPASS_READING_COMPLETED (1)
#define COMPASS_READING (0)
#define COMPASS_READING_FAILED (-1)

class Compass {
   private:
    uint8_t compassReadingStatus;
    unsigned long lastSuccessTime;
    int heading = -10;

   public:
    Compass() : lastSuccessTime(0), compassReadingStatus(COMPASS_READING_FAILED) {
        Wire.begin();  // conects I2C
        Wire.setWireTimeout(25000 /* us */, true /* reset_on_timeout */);
    }

    void update() {
        if (compassReadingStatus != COMPASS_READING) {
            Wire.beginTransmission(I2C_ADDRESS);
            Wire.write(0x2);
            Wire.endTransmission();

            Wire.clearWireTimeoutFlag();
            Wire.requestFrom(I2C_ADDRESS, 2);
            compassReadingStatus = COMPASS_READING;
        } else {
            if (Wire.available() < 2) {
                if (millis() - lastSuccessTime > 3000) {
                    heading = -1;
                    compassReadingStatus = COMPASS_READING_FAILED;
                }
                return;
            }
            uint8_t highByte = Wire.read();
            uint8_t lowByte = Wire.read();
            heading = ((highByte << 8) + lowByte);
            compassReadingStatus = COMPASS_READING_COMPLETED;
            lastSuccessTime = millis();
        }
    }

    int read() {
        return heading;
    }
};

Compass compass;

#endif /* _COMPASS_H_ */