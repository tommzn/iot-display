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

#include "contentmgr.h"
ContentManager contentManager(&display, &settings);

// Handler for incoming messages from AWS IOT.
void handleAwsIotMessage(String &topic, String &payload) {

  // Log rejected requests.
  if (topic.endsWith("rejected")) {
    Serial.println("Reject received, topic: " + topic);
    Serial.println("Reason: " + payload);
    return;
  } 

  // For any response to shadow get, process current shadow data.
  if (topic.endsWith("get/accepted")) {
    processDeviceShadow(payload);
    return;
  }

  // Process content and update displayed values if necessary.
  if (topic.endsWith("/contents")) {
    contentManager.processContent(payload);
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

  // If device shadow contains a delta, get deep sleep settings.
  JsonVariant deep_sleep = device_shadow["state"]["delta"]["deep_sleep_seconds"];
  if (!deep_sleep.isNull()) {
    uint32_t deep_sleep_seconds =  deep_sleep.as<uint32_t>();
    Serial.println("New deep sleep seconds: " + String(deep_sleep_seconds));
     settings.setSleepTime(deep_sleep_seconds);
  }
}

// Shutdown publishes current settings to device shadow, closes all connections
// and prepares/starts deep sleep.
void shutdown() {

  if (aws_iot_client.isConnected()) {

    // Report current  settings to device shadow.
    aws_iot_client.updateDeviceShadow(settings.getSleepTime());
    
    String logMessage = "Stop listening and going to deep sleep for " + String(settings.getSleepTime()) + " seconds.";
    aws_iot_client.publishInfoLogMessage(logMessage.c_str());
      
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

void setup() {

  // Init serial output
  Serial.begin(115200);
  Serial.println("");

  // Start settings manager
  settings.begin();

  // Try to connect to WiFi with given settings and credentials
  if (wifi.connect()) {

      aws_iot_client.begin();
      
      // Assign handler for incoming messages.
      aws_iot_client.handleMessage(handleAwsIotMessage);
        
      // Try to connect to AWS IOT.
      if (aws_iot_client.connect()) {
                
        // Subscribe to all required topics
        aws_iot_client.subsribe();
        aws_iot_client.publishInfoLogMessage("Subscribed to content topic. Waiting for updates.");

        // Trigger request for shadow data.
        aws_iot_client.triggerShadowGet();
        
        // Send message to initiate content publishing.
        aws_iot_client.triggerContentGet();
    
      }
  }

  display.init();             
  display.setTextColor(GxEPD_BLACK);
  
  // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  SPI.end();                  
  // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
  SPI.begin(13, 12, 14, 15);  

  // Runtime timer restricts time this device will wait for new contents.
  runtimeTimer.start(MAX_RUNTIME_SECONDS);
}

void loop() {

  delay(500);
  Serial.print("+");
  
  // Send and receive messages.
  aws_iot_client.loop();
  
  if (runtimeTimer.isExpired()) {

    // Stop processing and going to deep sleep.
    shutdown();
  }
}
