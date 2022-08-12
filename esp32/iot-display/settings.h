
#ifndef SETTINGS_H
#define SETTINGS_H


// Factors to convert seconds to milli and nicro settings
#define uS_TO_S_FACTOR 1000000ULL
#define mS_TO_S_FACTOR 1000

// Default sleep time, 10 min
#define SECONDS_TO_SLEEP 600

// Number of seconds the device listens for content updates
#define MAX_RUNTIME_SECONDS 30

// Max Wifi connect attemps
#define WIFI_MAX_CONNECT_ATTEMPS 10

// Max AWS IOT connect attemps
#define AWS_IOT_MAX_CONNECT_ATTEMPS 10

// AWS IOT settings
const char AWS_IOT_THING_NAME[]    = "<AWS IOT Thing Name>";
const char AWS_IOT_ENDPOINT[]      = "<your endpoint>-ats.iot.<region>.amazonaws.com";
const char AWS_IOT_LOG_TOPIC[]     = "logs";
const char AWS_IOT_CONTENT_TOPIC[] = "contents";

// Topics to interact with AWS IOT device shadow
const char AWS_IOT_SHADOW_GET_TOPIC[]           = "$aws/things/shadow-test/shadow/name/settings/get";
const char AWS_IOT_SHADOW_GET_ACCEPTED_TOPIC[]  = "$aws/things/shadow-test/shadow/name/settings/get/accepted";
const char AWS_IOT_SHADOW_UPDATE_TOPIC[]        = "$aws/things/<AWS IOT Thing Name>/shadow/name/settings/update";
const char AWS_IOT_SHADOW_REJECT_GET_TOPIC[]    = "$aws/things/shadow-test/shadow/name/settings/get/rejected";
const char AWS_IOT_SHADOW_REJECT_UPDATE_TOPIC[] = "$aws/things/shadow-test/shadow/name/settings/update/rejected";


#endif
  
