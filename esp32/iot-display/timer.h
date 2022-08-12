#include "settings.h"

#ifndef TIMER_H
#define TIMER_H


class Timer {
public:
  
  Timer() { };
  
  void start(int duration_seconds) {
    m_end_at = millis() + duration_seconds * mS_TO_S_FACTOR;
  };

  bool isExpired() {
    return millis() > m_end_at;
  };
  
private:
  unsigned long m_end_at;
};
#endif
