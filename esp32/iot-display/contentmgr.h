#include <GxEPD.h>
#include <GxGDEW0583T7/GxGDEW0583T7.h> // Waveshare 5.83" b/w

#include "nvs.h"

#ifndef CONTENTMGR_H
#define CONTENTMGR_H

// ContentManager will process received contents
// and update values on assigned display if necessary.
class ContentManager {
public:

  ContentManager(GxEPD_Class* display, Settings* settings) {
    m_display  = display;
    m_settings = settings;
  };

  void processContent(String& json);
  
private:

  GxEPD_Class* m_display;

  Settings* m_settings;

  void displayContent(JsonArray& items);
};
#endif
