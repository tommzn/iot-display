
#ifndef SETTINGS_H
#define SETTINGS_H


// Factors to convert seconds to milli and nicro settings
#define uS_TO_S_FACTOR 1000000ULL
#define mS_TO_S_FACTOR 1000

// Default sleep time, 10 min
#define SECONDS_TO_SLEEP 600

// Max Wifi connect attemps
#define WIFI_MAX_CONNECT_ATTEMPS 10

// Max AWS IOT connect attemps
#define AWS_IOT_MAX_CONNECT_ATTEMPS 10

// AWS IOT settings
const char AWS_IOT_THING_NAME[] = "<AWS IOT Thing Name>";
const char AWS_IOT_ENDPOINT[]   = "<your endpoint>-ats.iot.<region>.amazonaws.com";
const char AWS_IOT_LOG_TOPIC[]  = "iot-display/logs";

#endif
  
