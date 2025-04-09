#include <WiFi.h>
// #include <ESP8266WiFi.h>

#define LED_BUILTIN 2

const char *ap_ssid = "B_A";
const char *ap_password = "";

void setup() {
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    WiFi.softAP(ap_ssid, ap_password);

    Serial.print("IP address: ");
    Serial.println(WiFi.AP.localIP());
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1500);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(1500);                      // wait for a second
}
