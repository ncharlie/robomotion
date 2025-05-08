#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

byte sensorAddresses[] = {0x14, 0x12, 0x11, 0x13};  // Front, Back, Left, Right

#define ULTRASONIC_READING_COMPLETED (2)
#define ULTRASONIC_READING_REQUESTED (1)
#define ULTRASONIC_MEASURING (0)
#define ULTRASONIC_READING_FAILED (-1)

class Ultrasonic {
   private:
    uint8_t readingStatus;
    unsigned long requestTime;
    uint8_t measuringId;
    byte calculateChecksum(byte* data, int length) {
        byte sum = 0;
        for (int i = 0; i < length; i++) {
            sum += data[i];
        }
        return sum;
    }

   public:
    int distance[4];
    Ultrasonic() : readingStatus(ULTRASONIC_READING_FAILED) {
        Serial2.begin(19200, SERIAL_8N1);  // RS485
        Serial2.flush();
    }

    void update() {
        if (readingStatus == ULTRASONIC_READING_COMPLETED || readingStatus == ULTRASONIC_READING_FAILED) {
            for (int i = 0; i < 4; i++) {
                byte triggerData[7] = {0x55, 0xaa, sensorAddresses[i], 0x00, 0x01, 0x00};
                triggerData[5] = calculateChecksum(triggerData, 5);
                Serial2.write(triggerData, sizeof(triggerData));
            }
            requestTime = millis();
            readingStatus = ULTRASONIC_MEASURING;
        } else if (readingStatus == ULTRASONIC_MEASURING) {
            if (measuringId >= 4) {
                readingStatus = ULTRASONIC_READING_COMPLETED;
                return;
            }

            if (millis() - requestTime > 30) {
                byte requestData[7] = {0x55, 0xaa, sensorAddresses[measuringId], 0x00, 0x02, 0x00};
                requestData[5] = calculateChecksum(requestData, 5);
                Serial2.write(requestData, sizeof(requestData));
                Serial2.flush();
                requestTime = millis();
                // measuringId = (measuringId + 1) % 4;
            }
        }
    }

    void measure() {
        // 1. Trigger all sensors
        for (int i = 0; i < 4; i++) {
            byte triggerData[7] = {0x55, 0xaa, sensorAddresses[i], 0x00, 0x01, 0x00};
            triggerData[5] = calculateChecksum(triggerData, 5);
            Serial2.write(triggerData, sizeof(triggerData));
        }

        delay(30);  // Wait for all to measure
    }

    int read(int id) {
        // byte triggerData[7] = {0x55, 0xaa, sensorAddresses[id], 0x00, 0x01, 0x00};
        // triggerData[5] = calculateChecksum(triggerData, 5);
        // Serial2.write(triggerData, sizeof(triggerData));

        // delay(30);  // Wait for all to measure

        byte requestData[7] = {0x55, 0xaa, sensorAddresses[id], 0x00, 0x02, 0x00};
        requestData[5] = calculateChecksum(requestData, 5);
        Serial2.write(requestData, sizeof(requestData));
        Serial2.flush();

        delay(10);  // Allow sensor to respond

        if (Serial2.available() >= 8) {
            byte response[8];
            Serial2.readBytes(response, 8);
            if (response[0] == 0x55 && response[1] == 0xaa &&
                response[7] == calculateChecksum(response, 7)) {
                return (response[5] << 8) | response[6];
            }
        }
        return -1;

        // while (Serial2.available()) Serial2.read();  // Clear leftover bytes

        // return distance;
    }
};

Ultrasonic ultrasonic;

#endif /* _ULTRASONIC_H_ */