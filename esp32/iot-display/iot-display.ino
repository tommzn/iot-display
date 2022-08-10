/**
 * Content manager for e-Ink displays running on an ESP32.
 * 
 * Provides a content manager for e-Ink displays using am ESP32 with is ocnnected to 
 * AWS IOT. Settings are retieved from AWS IOT using device shadows. E-Ink content is
 * provided via MQTT topics in JSON format.
 * 
 * By tommzn <tommzn@gmx.de>
 * MIT License
 */

// Include AWS IOT devices certificates
#include "aws_iot_certs.h"

// Include WiFi credentials
#include "wifi_credentials.h"

// Include settings for Wifi and AWs IOT.
#include "settings.h"

// WiFi connection handler.
#include "WiFiConnection.h"
static WiFiConnection wifi = WiFiConnection(WIFI_SSID, WIFI_PASSWORD, MAX_WIFI_CONNECT_ATTEMPS);


void setup() {

  wifi.connect();
}

void loop() {

  delay(5000);
  wifi.disconnect();
}
