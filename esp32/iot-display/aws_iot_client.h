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
  void publishLogMessage(const char* message);

  // Handler for incoming messages.
  void handleMessage(MQTTClientCallbackSimpleFunction cb) {
    m_iot_client.onMessage(cb);
    triggerShadowGet();
  };

  // Send and receive messages.
  void loop() {
    m_iot_client.loop();
  }
  
private:

  // Client fo publish/subsribe to MQTT topics on AWS IOT.
  MQTTClient m_iot_client = MQTTClient(2048);

  // Secure client for AWS IOT connections using certificates.
  WiFiClientSecure m_secure_client = WiFiClientSecure();  

  // Number of attemps for connecting to AWS IOT.
  uint32_t m_max_connect_attemps = AWS_IOT_MAX_CONNECT_ATTEMPS;

  // AWS IOT thing name for this device.
  const char* m_thing_name = "<AWS IOT Thing Name>";

  // Name of used AWS IOT device shadow.
  const char* m_device_shadow_name = "settings";

  // Your AWS IOT Core endpoint. Region specific.
  const char* m_iot_endpoint = "<your endpoint>-ats.iot.<region>.amazonaws.com";

  // Topic log messages will be send to.
  const char* m_log_topic = "logs";

  // Topic this device subsribes to, to get new content for display updates.
  const char* m_content_topic = "contents";

  // Keep connection alive for given seconds, in case send/retrieve data take a little bit longer.
  uint32_t m_connection_keep_alive = 60;

  // Delay befreo "real" disconnect from AWS IOT to process all MQTT messages.
  uint32_t m_disconnect_delay = 1000;
  
  // Trigger GET topic for shadow device to initiate distribution of current shadow state.
  void triggerShadowGet() {
    m_iot_client.publish(getDeviceShadowTopic("/get"), "");
  };

  // Creates device shadow topics.
  // General format: $aws/things/<ThingName>/shadow/name/<ShadowName>/<Action>
  const char* getDeviceShadowTopic(std::string action) {
    return ("$aws/things/" + std::string(m_thing_name) + "/shadow/name/" + std::string(m_device_shadow_name) + action).c_str();
  }
  
  
};
#endif
