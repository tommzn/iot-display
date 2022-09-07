#include <ArduinoJson.h>

#include <GxEPD.h>
#include <GxGDEW0583T7/GxGDEW0583T7.h> // Waveshare 5.83" b/w

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#ifndef DISPLAY_H
#define DISPLAY_H

class DisplayManager {
public:

  DisplayManager() {

    
    /**
     * From https://www.waveshare.com/wiki/E-Paper_ESP32_Driver_Board
     * CS   P15 Chip Select, active-low
     * BUSY P25 Busy Output Pin (means busy)
     * RST  P26 Reset, active-low
     * DC   P27 Reset, Data/Demand, low level means to demand, high level means data
     */
    int8_t pin_rst  = 26;
    int8_t pin_busy = 25;
    int8_t pin_cs   = 15;
    int8_t pin_dc   = 27;
    GxIO_Class io(SPI, pin_cs, pin_dc, pin_rst);
    m_display = new GxEPD_Class(io, pin_rst, pin_busy);
  };
  
  void drawContent(JsonArray& items);

  void init() {

    // enable diagnostic output on Serial by passing baud rate
    //display.init(115200);
    m_display->init(); 
  };

  void end() {
    m_display->powerDown();
  };
  
private:

  GxEPD_Class* m_display;


};
#endif
