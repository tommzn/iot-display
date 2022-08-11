#include "WiFiClientSecure.h"
#include "MQTTClient.h"

#include "settings.h"

#ifndef AWS_IOT_CLIENT_H
#define AWS_IOT_CLIENT_H

class AwsIotClient {
public:

  AwsIotClient() { };

  // Opens a secure connections to AWS IOT to publish/consume messages.
  bool connect();

  // Closed current connection to AWS IOT.
  bool disconnect();

  // Writes given message to defined log topic.
  // See AWS_IOT_LOG_TOPIC in settings.h
  void publishLog(const char* message);
  
private:

  // Client fo publish/subsribe to MQTT topics on AWS IOT.
  MQTTClient m_iot_client = MQTTClient(2048);

  // Secure client for AWS IOT connections using certificates.
  WiFiClientSecure m_secure_client = WiFiClientSecure();  

  // Number of attemps for connecting to AWS IOT.
  uint32_t m_max_connect_attemps = AWS_IOT_MAX_CONNECT_ATTEMPS;

  // AWS IOT thing name for this device.
  const char* m_thing_name = AWS_IOT_THING_NAME;

  // Your AWS IOT Core endpoint. Region specific.
  const char* m_iot_endpoint = AWS_IOT_ENDPOINT;

  // Topic log messages will be send to.
  const char* m_log_topic = AWS_IOT_LOG_TOPIC;

  // Keep connection alive for given seconds, in case send/retrieve data take a little bit longer.
  uint32_t m_connection_keep_alive = 60;

  // Delay befreo "real" disconnect from AWS IOT to process all MQTT messages.
  uint32_t m_disconnect_delay = 1000;
  
};
#endif
