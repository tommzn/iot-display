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
#include <Preferences.h>

// Include AWS IOT devices certificates
#include "aws_iot_certs.h"

// Include WiFi credentials
#include "wifi_credentials.h"

// Include settings for Wifi and AWS IOT
#include "settings.h"

// Include persistence mananger for settings.
#include "nvs.h"

// WiFi connection handler.
#include "WiFiConnection.h"
static WiFiConnection wifi = WiFiConnection(WIFI_SSID, WIFI_PASSWORD, WIFI_MAX_CONNECT_ATTEMPS);

// Create new settings manager.
Settings settings(new Preferences(), SECONDS_TO_SLEEP);

// Will start deep sleep for defined number of seconds
void enterDeepSleep() {

  delay(1000);

  uint32_t secondsToSleep = settings.getSleepTime();
  Serial.println("Setup ESP32 deep sleep for " + String(secondsToSleep) + " seconds.");
  esp_sleep_enable_timer_wakeup(secondsToSleep * uS_TO_S_FACTOR);

  Serial.println("Going to deep sleep now!");
  Serial.flush(); 
  esp_deep_sleep_start();
}

void setup() {

  // Init serial output
  Serial.begin(115200);
  Serial.println("");

  
  // Start settings manager
  settings.begin();

  // Try to connect to WiFi with given settings and credentials
  wifi.connect();
}

void loop() {

  delay(500);

  // Disconnect from WiFi
  wifi.disconnect();

  // Close settings manager
  settings.end();

  // Going to deep sleep for defined number of seconds
  // See SECONDS_TO_SLEEP in settings.h for default value
  enterDeepSleep();
}
