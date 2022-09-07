#include "Arduino.h"
#include <ArduinoJson.h>

// FreeFonts from Adafruit_GFX
#include <Adafruit_GFX.h> 
//#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
//#include <Fonts/FreeMonoBold18pt7b.h>
//#include <Fonts/FreeMonoBold24pt7b.h>

#include "display.h"


void DisplayManager::drawContent(JsonArray& items) {

  if (items.size() == 0) {
    return;
  }
  
  m_display->setFont(&FreeMonoBold12pt7b);
  m_display->setTextColor(GxEPD_BLACK);
  m_display->fillScreen(GxEPD_WHITE);

  for (JsonArray::iterator it=items.begin(); it!=items.end(); ++it) {

    JsonObject item = it->as<JsonObject>();
    if (item.containsKey("text") && item.containsKey("position")) { 

      uint16_t x = item["position"]["x"];
      uint16_t y = item["position"]["y"];
      Serial.println("Position, X: " + String(x) + ", Y: " + String(y));
    
      String text = item["text"];
      Serial.println("Text: " + text);
    
      m_display->setCursor(x, y);
      m_display->print(text);
      
    } else {
      Serial.println(F("Missing position or text"));
    }
  }

  Serial.println(F("Start display update"));
  m_display->update();
  Serial.println(F("End display update"));
}
