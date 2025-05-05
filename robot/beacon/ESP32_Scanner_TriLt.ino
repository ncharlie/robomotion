#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

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

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.haveManufacturerData()) {
        // FIX: Use String instead of std::string
        String data = advertisedDevice.getManufacturerData();
        
        if (data.length() == sizeof(anchor_adv_data_t)) {
          const anchor_adv_data_t* adv = (const anchor_adv_data_t*)data.c_str(); // Convert to C-string
          
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
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop() {
  resetAnchors();
  
  // FIX: Use pointer correctly (BLEScanResults is a class)
  BLEScanResults* pResults = pBLEScan->start(2, false);
  
  if (allAnchorsUpdated()) {
    float x, y;
    if (trilaterate(x, y)) {
      Serial.print("Position: ");
      Serial.print(x);
      Serial.print(",");
      Serial.println(y);
    }
  }
  
  pBLEScan->clearResults(); // Free memory
  delay(1000);
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
    Serial.println("Trilateration failed: anchors might be collinear.");
    return false;
  }

  x = (C * E - B * F) / denominator;
  y = (A * F - C * D) / denominator;

  return true;
}
