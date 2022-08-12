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

// Include WiFi credentials.
#include "wifi_credentials.h"

// Include settings for Wifi and AWS IOT.
#include "settings.h"

// Include persistence mananger for settings.
#include "nvs.h"

// Include and create AWS IOT client.
#include "aws_iot_client.h"
AwsIotClient aws_iot_client;

// Include and create WiFi connection handler.
#include "WiFiConnection.h"
static WiFiConnection wifi = WiFiConnection(WIFI_SSID, WIFI_PASSWORD, WIFI_MAX_CONNECT_ATTEMPS);

// Create new settings manager.
Settings settings(new Preferences(), SECONDS_TO_SLEEP);

// Runtime timer to restrict time this device it active and waiting ofr incoming messages.
#include "timer.h"
Timer runtimeTimer;

// Handler for incoming messages from AWS IOT.
void handleAwsIotMessage(String &topic, String &payload) {

  if (topic.endsWith("rejected")) {
    Serial.println("Reject received, topic: " + topic);
    Serial.println("Reason: " + payload);
    return;
  } 

  if (topic.endsWith("get/accepted")) {
    Serial.println("Shadow data received, topic: " + topic);
    Serial.println("Content: " + payload);
    return;
  } 
  Serial.println("incoming: " + topic + " - " + payload);
}

// Will start deep sleep for defined number of seconds.
// Default sleep time is defined by SECONDS_TO_SLEEP.
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

  // After WiFi connectin is establishes...
  if (wifi.connected()) {

      // ...try to connect to AWS IOT.
      if (aws_iot_client.connect()) {
        Serial.println("Connected to AWS IOT!");
        // Assign handler for incoming messages.
        aws_iot_client.handleMessage(handleAwsIotMessage);
        aws_iot_client.publishLogMessage("Subscribed to content topic. Waiting for updates.");
      } else {
        Serial.println("Unable to connect to AWS IOT!");
      }
  }

  runtimeTimer.start(MAX_RUNTIME_SECONDS);
}

void loop() {

  delay(500);
  // Send and receive messages.
  aws_iot_client.loop();
  
  // If runtime exceeds, shut down.
  if (runtimeTimer.isExpired()) {

    String logMessage = "Stop listening and going to deep sleep for " + String(settings.getSleepTime()) + " seconds.";
    aws_iot_client.publishLogMessage(logMessage.c_str());
    
    // @ToDo: Publish current settings to device shadow.
    
    // Disconnect from AWS IOT
    aws_iot_client.disconnect();
    
    // Disconnect from WiFi
    wifi.disconnect();
  
    // Close settings manager
    settings.end();
  
    // Going to deep sleep for defined number of seconds
    // See SECONDS_TO_SLEEP in settings.h for default value
    enterDeepSleep();
  }
}
