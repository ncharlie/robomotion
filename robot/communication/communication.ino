#include <map>
#include <vector>

#include <TFT_eSPI.h>  // TFT display library
#include <WiFi.h>
#include "PubSubClient.h"
#include "beacon.h"
#include "rssi.h"

const char* LocationTopic = "@msg/location/robot1";
const char* NotificationTopic = "@msg/noti/robot1";
const char* CommandTopic = "@msg/move/robot1";

std::map<String, Beacon> beacons;
TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display

void displayMessage(const char* message, int y, uint16_t color = TFT_WHITE) {
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(message, 10, y, 2);
}

void setup() {
    beacons.insert_or_assign("B_A", Beacon(1, 2, 3));
    beacons.insert_or_assign("B_B", Beacon(4, 5, 6));
    beacons.insert_or_assign("B_C", Beacon(7, 8, 9));
    beacons.insert_or_assign("B_D", Beacon(0, 1, 2));
    // beacons["B_B"] = Beacon(4,5,6);
    Serial.begin(19200);
    delay(10);

    // Initialize TFT display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    displayMessage("Radix Scanner", 0, TFT_CYAN);

    WiFi.mode(WIFI_STA);
}

void loop() {
    // put your main code here, to run repeatedly:
    int16_t WiFiScanStatus = WiFi.scanComplete();
    int row = 40;

    if (WiFiScanStatus < 0) {  // it is busy scanning or got an error
        if (WiFiScanStatus == WIFI_SCAN_FAILED) {
            // Serial.println("WiFi Scan has failed. Starting again.");
            displayMessage("Scan failed!", row, TFT_RED);
            WiFi.scanNetworks(true, false, false, 250, 1);
        }
        // other option is status WIFI_SCAN_RUNNING - just wait.
        return;
    }

    // Found Zero or more Wireless Networks
    std::vector<Rssi> data;
    for (int i = 0; i < WiFiScanStatus; ++i) {
        // mark rssi
        if (beacons.count(WiFi.SSID(i))) {
            data.push_back(Rssi(WiFi.SSID(i), WiFi.RSSI(i)));
        }
    }

    for (auto& element : data) {
        // Serial.printf("%s: %d\n", element.name, element.rssi);

        // auto & b = beacons[element.name];
        // Serial.printf("location: (%d, %d, %d)\n", beacons[element.name].x, beacons[element.name].y, beacons[element.name].z);

        char msg[256];
        sprintf(msg, "%s: %d\n", element.name, element.rssi);
        displayMessage(msg, row, TFT_GREEN);
        row += 40;
    }

    WiFi.scanNetworks(true);
}
