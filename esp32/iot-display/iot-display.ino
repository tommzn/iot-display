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

// Include settings for Wifi and AWS IOT
#include "settings.h"

// WiFi connection handler.
#include "WiFiConnection.h"
static WiFiConnection wifi = WiFiConnection(WIFI_SSID, WIFI_PASSWORD, WIFI_MAX_CONNECT_ATTEMPS);

// Will start deep sleep for defined number of seconds
void enterDeepSleep() {

  delay(1000);
  
  Serial.println("Setup ESP32 deep sleep for " + String(SECONDS_TO_SLEEP) + " seconds.");
  esp_sleep_enable_timer_wakeup(SECONDS_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("Going to deep sleep now!");
  Serial.flush(); 
  esp_deep_sleep_start();
}

void setup() {

  // Try to connect to WiFi with given settings and credentials
  wifi.connect();
}

void loop() {

  delay(500);

  // Disconnect from WiFi
  wifi.disconnect();

  // Going to deep sleep for defined number of seconds
  // See SECONDS_TO_SLEEP in settings.h for default value
  enterDeepSleep();
}
