#include "settings.h"

#ifndef TIMER_H
#define TIMER_H


// Timer can be used to trace a time range.
// You can set a duration in seconds and check if this time range is expired in the meantime.
class Timer {
public:
  
  Timer() { };
  
  void start(uint8_t duration_seconds) {
    m_end_at = millis() + duration_seconds * mS_TO_S_FACTOR;
  };

  bool isExpired() {
    return millis() > m_end_at;
  };
  
private:
  unsigned long m_end_at;
};
#endif
