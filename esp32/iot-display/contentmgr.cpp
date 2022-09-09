#include "Arduino.h"
#include <ArduinoJson.h>

#include <GxEPD.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#include "contentmgr.h"

void ContentManager::processContent(String& json) {

  StaticJsonDocument<1024> content;
  DeserializationError error = deserializeJson(content, json);
  if (error) {
    Serial.println(F("Unable to parse content"));
    Serial.println(error.f_str());
    return;
  }
  
  // If content contains a hash, compared to local hash
  if (content.containsKey("content_hash")) { 

    String contentHash = content["content_hash"];
    String localContentHash = m_settings->getContentHash();
    if (localContentHash == contentHash) {
      Serial.println(F("Content has not changed. No update required."));
      return;
    }

    // Persist new content hash for future updates.
    m_settings->setContentHash(contentHash);
  }

  if (content.containsKey("items")) { 
    JsonArray items = content["items"].as<JsonArray>();
    displayContent(items);
  }
}

void ContentManager::displayContent(JsonArray& items) {

  if (items.size() == 0) {
    return;
  }
  
  m_display->setFont(&FreeMonoBold9pt7b);
  m_display->fillScreen(GxEPD_WHITE);

  for (JsonArray::iterator it=items.begin(); it!=items.end(); ++it) {

    JsonObject item = it->as<JsonObject>();
    String text = item["text"];
    uint16_t x = item["position"]["x"];
    uint16_t y = item["position"]["y"]; 
    if (text && x && y) { 
      m_display->setCursor(x, y);
      m_display->print(text);
    }
  }
  m_display->update();
}
