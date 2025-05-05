// Function to calculate checksum
byte calculateChecksum(byte *data, int length) {
    byte sum = 0;
    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

// Function to map address to label
const char* getSensorLabel(byte address) {
    switch (address) {
        case 0x14: return "F";  // Front
        case 0x13: return "R";  // Right
        case 0x11: return "L";  // Left
        case 0x12: return "B";  // Back
        default: return "Unknown";
    }
}

byte sensorAddresses[] = {0x11, 0x12, 0x13, 0x14};  // Left, Back, Right, Front

void setup() {
    Serial.begin(9600);               // Monitor
    Serial2.begin(19200, SERIAL_8N1); // RS485
    Serial2.flush();
    Serial.println("Ultrasonic Sensor Reading (Trigger)");
}

void loop() {
    // 1. Trigger all sensors
    for (int i = 0; i < 4; i++) {
        byte addr = sensorAddresses[i];
        byte triggerData[7] = {0x55, 0xaa, addr, 0x00, 0x01, 0x00};
        triggerData[5] = calculateChecksum(triggerData, 5);
        Serial2.write(triggerData, sizeof(triggerData));
    }

    delay(30);  // Wait for all to measure

    // 2. Request + Read one-by-one
    for (int i = 0; i < 4; i++) {
        byte addr = sensorAddresses[i];
        byte requestData[7] = {0x55, 0xaa, addr, 0x00, 0x02, 0x00};
        requestData[5] = calculateChecksum(requestData, 5);
        Serial2.write(requestData, sizeof(requestData));
        Serial2.flush();

        delay(20);  // Allow sensor to respond

        int distance = -1;
        byte response[8];
        unsigned long start = millis();
        while (Serial2.available() < 8 && millis() - start < 50); // timeout safety

        if (Serial2.available() >= 8) {
            Serial2.readBytes(response, 8);
            byte receivedChecksum = response[7];
            byte calculatedChecksum = calculateChecksum(response, 7);

            if (response[0] == 0x55 && response[1] == 0xaa && receivedChecksum == calculatedChecksum) {
                uint16_t rawDistance = (response[5] << 8) | response[6];

                if (rawDistance != 0xFFFF && rawDistance > 0 && rawDistance < 250) {
                    distance = rawDistance;
                }
            } else {
                Serial.println("Checksum or header error");
            }
        } else {
            Serial.println("No response from sensor");
        }

        // Print result
        Serial.print("Sensor ");
        Serial.print(getSensorLabel(addr));
        Serial.print(": ");
        if (distance == -1) {
            Serial.println("Invalid or out of range");
        } else {
            Serial.print(distance);
            Serial.println(" cm");

            if (distance <= 10) {
                Serial.println(" Obstacle detected!");
            }
        }

        while (Serial2.available()) Serial2.read();  // Clear leftover bytes
    }

    Serial.println("---");
    delay(200);  // Optional cycle delay
}
