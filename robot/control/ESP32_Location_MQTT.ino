#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <WiFi.h>
#include <PubSubClient.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t manufacturer_id;
    uint8_t anchor_id;
    float pos_x;
    float pos_y;
    int8_t tx_power;
} anchor_adv_data_t;
#pragma pack(pop)

struct Anchor {
  float x;
  float y;
  float distance;
  bool updated;
} anchors[3];

BLEScan* pBLEScan;
const float PATH_LOSS_EXPONENT = 2.0;

// Wi-Fi credentials
const char* ssid = "Chas";
const char* password = "charliee";

// MQTT Broker details
const char* mqtt_server = "mqtt.netpie.io";  // Change this to your broker address
const int mqtt_port = 1883;  // Standard MQTT port
const char* mqtt_clientId = "b63e218b-07eb-4f6c-acc2-03d0150a333b";  // Provided clientId
const char* mqtt_token = "YsqYnqFncLzhS2HZd6rELECP5VCjggVt";  // Provided token
const char* mqtt_secret = "rnxWyWRkdnrVoby8SqwEwzGa1sbhgGd9";  // Provided secret

WiFiClient espClient;
PubSubClient client(espClient);

// Define the topic to publish to
const char* topic = "@msg/location/r1";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.haveManufacturerData()) {
        String data = advertisedDevice.getManufacturerData();
        
        if (data.length() == sizeof(anchor_adv_data_t)) {
          const anchor_adv_data_t* adv = (const anchor_adv_data_t*)data.c_str(); 
          
          if (adv->manufacturer_id == 0x1234 && adv->anchor_id >= 1 && adv->anchor_id <= 3) {
            int rssi = advertisedDevice.getRSSI();
            anchors[adv->anchor_id-1] = {
              .x = adv->pos_x,
              .y = adv->pos_y,
              .distance = pow(10, (adv->tx_power - rssi)/(10 * PATH_LOSS_EXPONENT)),
              .updated = true
            };
            Serial.printf("Anchor %d: %.2fm\n", adv->anchor_id, anchors[adv->anchor_id-1].distance);
          }
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // Initialize BLE
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
//    Serial.println("Connecting to WiFi...");
  }
//  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  
  // Attempt to connect to the MQTT broker with provided credentials
  while (!client.connected()) {
//    Serial.print("Connecting to MQTT...");
    if (client.connect(mqtt_clientId, mqtt_token, mqtt_secret)) {
//      Serial.println("Connected to MQTT");
    } else {
//      Serial.print("Failed to connect, retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  resetAnchors();
  
  // Start BLE scan
  BLEScanResults* pResults = pBLEScan->start(2, false);
  
  if (allAnchorsUpdated()) {
    float x, y;
    if (trilaterate(x, y)) {
      Serial.print("Position: ");
      Serial.print(x);
      Serial.print(",");
      Serial.println(y);

      // Publish the X, Y position to MQTT
      String payload = "X: " + String(x) + ", Y: " + String(y);
      client.publish(topic, payload.c_str());
    }
  }
  
  pBLEScan->clearResults(); // Free memory
  delay(1000);
  client.loop();  // Ensure MQTT keeps running
}

bool allAnchorsUpdated() {
  return anchors[0].updated && anchors[1].updated && anchors[2].updated;
}

void resetAnchors() {
  for (int i = 0; i < 3; i++) {
    anchors[i].updated = false;
  }
}

  bool trilaterate(float &x, float &y) {
  float x1 = anchors[0].x, y1 = anchors[0].y, r1 = anchors[0].distance;
  float x2 = anchors[1].x, y2 = anchors[1].y, r2 = anchors[1].distance;
  float x3 = anchors[2].x, y3 = anchors[2].y, r3 = anchors[2].distance;

  // Transform to linear equations: (x - xi)^2 + (y - yi)^2 = ri^2
  // Subtract pairwise to eliminate squared terms

  float A = 2 * (x2 - x1);
  float B = 2 * (y2 - y1);
  float C = r1*r1 - r2*r2 - x1*x1 + x2*x2 - y1*y1 + y2*y2;

  float D = 2 * (x3 - x2);
  float E = 2 * (y3 - y2);
  float F = r2*r2 - r3*r3 - x2*x2 + x3*x3 - y2*y2 + y3*y3;

  float denominator = A * E - B * D;

  if (fabs(denominator) < 1e-6) {
    // Anchors are collinear or badly placed
//    Serial.println("Trilateration failed: anchors might be collinear.");
    return false;
  }

  x = (C * E - B * F) / denominator;
  y = (A * F - C * D) / denominator;

  return true;
}
