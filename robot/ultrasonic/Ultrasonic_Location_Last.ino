// Room dimensions (cm)
const float ROOM_WIDTH = 200.0;
const float ROOM_LENGTH = 200.0;

// Sensor data storage
float robotX = 0, robotY = 0;
int frontDist = -1, backDist = -1, leftDist = -1, rightDist = -1;

void setup() {
  Serial.begin(115200);    // Debug console
  Serial3.begin(115200);   // ESP32 communication (TX3=14, RX3=15)
  Serial2.begin(19200);    // RS485 sensors (TX2=16, RX2=17)
}

byte calculateChecksum(byte *data, int length) {
  byte sum = 0;
  for (int i = 0; i < length; i++) sum += data[i];
  return sum;
}

int readDistance(byte address) {
  byte trigger[7] = {0x55, 0xaa, address, 0x00, 0x01, 0x00};
  trigger[5] = calculateChecksum(trigger, 5);
  Serial2.write(trigger, sizeof(trigger));
  delay(30);
  
  byte request[7] = {0x55, 0xaa, address, 0x00, 0x02, 0x00};
  request[5] = calculateChecksum(request, 5);
  Serial2.write(request, sizeof(request));
  delay(20);

  if (Serial2.available() >= 8) {
    byte response[8];
    Serial2.readBytes(response, 8);
    if (response[0] == 0x55 && response[1] == 0xaa && 
        response[7] == calculateChecksum(response, 7)) {
      return (response[5] << 8) | response[6];
    }
  }
  return -1;
}

void updatePosition() {
  struct Sensor {
    const char* label;
    int value;
  };

  Sensor sensors[] = {
    {"L", leftDist},
    {"R", rightDist},
    {"F", frontDist},
    {"B", backDist}
  };

  // Manual bubble sort (ascending by value)
  for (int i = 0; i < 3; i++) {
    for (int j = i + 1; j < 4; j++) {
      if (sensors[i].value > sensors[j].value) {
        Sensor temp = sensors[i];
        sensors[i] = sensors[j];
        sensors[j] = temp;
      }
    }
  }
  
  float x = -1, y = -1;
  
  if (frontDist == -1 && backDist == -1) return;
  else {
    if (frontDist != -1 && frontDist <= backDist) y = ROOM_LENGTH - frontDist;
    else if (backDist != -1 && backDist <= frontDist) y = backDist;
  }
  if (leftDist == -1 && rightDist == -1) return;
  else {
    if (rightDist != -1 && rightDist <= leftDist) x = ROOM_LENGTH - rightDist;
    else if (leftDist != -1 && leftDist <= rightDist) x = leftDist;
  }


  // Use the two nearest sensor values
//  for (int i = 0; i < 2; ++i) {
//    String side = sensors[i].label;
//    int dist = sensors[i].value;
//
//    if (side == "L") x = dist;
//    if (side == "R") x = ROOM_WIDTH - dist;
//    if (side == "F") y = ROOM_LENGTH - dist;
//    if (side == "B") y = dist;
//  }

  if (x >= 0) robotX = x;
  if (y >= 0) robotY = y;

  // Clamp within room bounds
  robotX = constrain(robotX, 0, ROOM_WIDTH);
  robotY = constrain(robotY, 0, ROOM_LENGTH);
}


static byte sensorAddr = 0x11; // Start with left sensor

void loop() {
  int dist = readDistance(sensorAddr);

  if (dist > 250) dist = -1;
  
  switch(sensorAddr) {
    case 0x11: leftDist = dist; break;
    case 0x12: backDist = dist; break;
    case 0x13: rightDist = dist; break;
    case 0x14: frontDist = dist; break;
  }

  if (sensorAddr >= 0x14) {
    updatePosition();

    String data = "{\"x\":" + String(robotX, 1) + 
                 ",\"y\":" + String(robotY, 1) + 
                 ",\"F\":" + frontDist + 
                 ",\"B\":" + backDist + 
                 ",\"L\":" + leftDist + 
                 ",\"R\":" + rightDist + "}";
    Serial3.println(data);
    Serial.println(data);
    sensorAddr = 0x11;
  } else {
    sensorAddr++;
  }

  delay(100);
}
