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
#include "aws_iot_settings.h";
AwsIotClient aws_iot_client = AwsIotClient(AWS_IOT_ENDPOINT, AWS_IOT_THING_NAME, AWS_IOT_MAX_CONNECT_ATTEMPS);

// Include and create WiFi connection handler.
#include "WiFiConnection.h"
static WiFiConnection wifi = WiFiConnection(WIFI_SSID, WIFI_PASSWORD, WIFI_MAX_CONNECT_ATTEMPS);

// Create new settings manager.
Preferences preferences;
Settings settings(&preferences, SECONDS_TO_SLEEP);

// Runtime timer to restrict time this device it active and waiting ofr incoming messages.
#include "timer.h"
Timer runtimeTimer;

// Display setup
#include <GxEPD.h>
#include <GxGDEW0583T7/GxGDEW0583T7.h> // Waveshare 5.83" b/w

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

GxIO_Class io(SPI, 15, 27, 26);
GxEPD_Class display(io, 26, 25);

// Handler for incoming messages from AWS IOT.
void handleAwsIotMessage(String &topic, String &payload) {

  if (topic.endsWith("rejected")) {
    Serial.println("Reject received, topic: " + topic);
    Serial.println("Reason: " + payload);
    return;
  } 

  if (topic.endsWith("get/accepted")) {
    processDeviceShadow(payload);
    return;
  }

   if (topic.endsWith("/contents")) {
    processContent(payload);
    return;
  }
}

void processDeviceShadow(String &json) {

  StaticJsonDocument<512> device_shadow;
  DeserializationError error = deserializeJson(device_shadow, json);
  if (error) {
    Serial.println(F("Unable to parse device shadow"));
    Serial.println(error.f_str());
    return;
  }

  JsonObject deviceState = device_shadow["state"];
  // If device shadow contains a delta, get deep sleep settings.
  if (deviceState.containsKey("delta")) { 

    JsonObject deltaState = deviceState["delta"];
    if (deltaState.containsKey("deep_sleep_seconds")) {

      uint32_t deep_sleep_seconds = deltaState["deep_sleep_seconds"];
      Serial.println("New deep sleep seconds: " + String(deep_sleep_seconds));
      settings.setSleepTime(deep_sleep_seconds);
    }
  }
}

void processContent(String &contentAsJson) {

  StaticJsonDocument<1024> content;
  DeserializationError error = deserializeJson(content, contentAsJson);
  if (error) {
    Serial.println(F("Unable to parse content"));
    Serial.println(error.f_str());
    return;
  }
  
  // If content contains a hash, compared to local hash
  if (content.containsKey("content_hash")) { 

    String contentHash = content["content_hash"];
    String localContentHash = settings.getContentHash();
    if (localContentHash == contentHash) {
      Serial.println(F("Content has not changed. No update required."));
      return;
    }

    // Persist new content hash for future updates.
    settings.setContentHash(contentHash);
  }

  if (content.containsKey("items")) { 
    JsonArray items = content["items"].as<JsonArray>();
    drawContent(items);
  }
}

// Will start deep sleep for defined number of seconds.
// Default sleep time is defined by SECONDS_TO_SLEEP.
void enterDeepSleep(uint32_t secondsToSleep) {

  delay(1000);

  Serial.println("Setup ESP32 deep sleep for " + String(secondsToSleep) + " seconds.");
  esp_sleep_enable_timer_wakeup(secondsToSleep * uS_TO_S_FACTOR);

  Serial.println("Going to deep sleep now!");
  Serial.flush(); 
  esp_deep_sleep_start();
}

// drawContent will iterate trough passed items and draw their text on screen.
void drawContent(JsonArray& items) {

  if (items.size() == 0) {
    return;
  }
  
  display.setFont(&FreeMonoBold9pt7b);
  display.fillScreen(GxEPD_WHITE);

  for (JsonArray::iterator it=items.begin(); it!=items.end(); ++it) {

    JsonObject item = it->as<JsonObject>();
    if (item.containsKey("text") && item.containsKey("position")) { 

      uint16_t x = item["position"]["x"];
      uint16_t y = item["position"]["y"];
      String text = item["text"];
      display.setCursor(x, y);
      display.print(text);
    }
  }
  display.update();
  
}

void setup() {

  // Init serial output
  Serial.begin(115200);
  Serial.println("");

  // Start settings manager
  settings.begin();

  // Try to connect to WiFi with given settings and credentials
  wifi.connect();

  // After WiFi connectin is established...
  if (wifi.connected()) {

      aws_iot_client.begin();
      
      // Assign handler for incoming messages.
      aws_iot_client.handleMessage(handleAwsIotMessage);
        
      // Ttry to connect to AWS IOT.
      Serial.println("Connecting to AWS IOT.");
      if (aws_iot_client.connect()) {
        
        aws_iot_client.publishInfoLogMessage("Subscribed to content topic. Waiting for updates.");

        // Subscribe to all required topics
        aws_iot_client.subsribe();

        // Trigger request for shadow data.
        aws_iot_client.triggerShadowGet();
        
        // Send message to initiate content publishing
        aws_iot_client.triggerContentGet();
    
      }
  }

  display.init();             
  display.setTextColor(GxEPD_BLACK);
  
  // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  SPI.end();                  
  // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
  SPI.begin(13, 12, 14, 15);  
  
  runtimeTimer.start(MAX_RUNTIME_SECONDS);
}

void loop() {

  delay(500);
  Serial.print("+");
  
  // Send and receive messages.
  aws_iot_client.loop();
  
  // If runtime exceeds, shut down.
  if (runtimeTimer.isExpired()) {

    Serial.println("");
    if (aws_iot_client.isConnected()) {
      
      String logMessage = "Stop listening and going to deep sleep for " + String(settings.getSleepTime()) + " seconds.";
      aws_iot_client.publishInfoLogMessage(logMessage.c_str());
      
      // @ToDo: Publish current settings to device shadow.

      // Report current  settings to device shadow.
      aws_iot_client.updateDeviceShadow(settings.getSleepTime());
        
      // Disconnect from AWS IOT
      aws_iot_client.disconnect();
    }
    
    // Disconnect from WiFi
    wifi.disconnect();
  
    // Close settings manager
    uint32_t secondsToSleep = settings.getSleepTime();
    settings.end();

    // Power off display
    display.powerDown();

    // Going to deep sleep for defined number of seconds
    // See SECONDS_TO_SLEEP in settings.h for default value
    enterDeepSleep(secondsToSleep);
  }
}
