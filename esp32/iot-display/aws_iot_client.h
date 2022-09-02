#include "WiFiClientSecure.h"
#include "MQTTClient.h"

#include "settings.h"

#ifndef AWS_IOT_CLIENT_H
#define AWS_IOT_CLIENT_H

class AwsIotClient {
public:

  AwsIotClient(const char* iot_endpoint, const char* thing_name, uint8_t max_connect_attemps) { 
    m_iot_endpoint = iot_endpoint;
    m_thing_name   = thing_name;
    m_max_connect_attemps = max_connect_attemps;

    std::string topic_path = std::string(m_topic_namespace) + "/things/" + std::string(m_thing_name);
    m_content_topic = topic_path + "/contents";
    m_log_topic     = topic_path + "/logs";
  };

  // Opens a secure connections to AWS IOT to publish/consume messages.
  bool connect();

  // Closed current connection to AWS IOT.
  bool disconnect();

  // Get connection status.
  bool isConnected() {
    return m_iot_client.connected();
  };

  // Writes given message to info log topic.
  // See AWS_IOT_LOG_TOPIC in settings.h
  void publishInfoLogMessage(const char* message) {
    m_iot_client.publish((m_log_topic + "/info").c_str(), message);
  };

  // Writes given message to error log topic.
  // See AWS_IOT_LOG_TOPIC in settings.h
  void publishErrorLogMessage(const char* message) {
    m_iot_client.publish((m_log_topic + "/error").c_str(), message);
  };

  // Handler for incoming messages.
  void handleMessage(MQTTClientCallbackSimpleFunction cb) {
    m_iot_client.onMessage(cb);
    //triggerShadowGet();
  };

  // Send and receive messages.
  void loop() {
    m_iot_client.loop();
  };

  void logError() {
    Serial.print("MQTT Connect Error: ");
    Serial.println(m_iot_client.lastError());  
  };
  
private:

  // Client fo publish/subsribe to MQTT topics on AWS IOT.
  MQTTClient m_iot_client = MQTTClient(2048);

  // Secure client for AWS IOT connections using certificates.
  WiFiClientSecure m_secure_client = WiFiClientSecure();  

  // Number of attemps for connecting to AWS IOT.
  uint8_t m_max_connect_attemps;

  // Your AWS IOT Core endpoint. Region specific.
  const char* m_iot_endpoint;

  // AWS IOT thing name for this device.
  const char* m_thing_name;

  // Name of used AWS IOT device shadow.
  const char* m_device_shadow_name = "settings";

  // Namespace/prefix for all log and content topics.
  const char* m_topic_namespace = "iotdisplay";
  
  // Topic this device subsribes to, to get new content for display updates.
  std::string m_content_topic;

  // Topic used for sending log messages.
  std::string m_log_topic;
  
  // Keep connection alive for given seconds, in case send/retrieve data take a little bit longer.
  uint8_t m_connection_keep_alive = 60;

  // Delay in seconds befreo "real" disconnect from AWS IOT to process all MQTT messages.
  uint8_t m_disconnect_delay = 3;

  // Trigger GET topic for shadow device to initiate distribution of current shadow state.
  void triggerShadowGet() {
    m_iot_client.publish(getDeviceShadowTopic("/get"), "");
  };

  // Trigger GET topic for contents.
  void triggerContentGet() {
    m_iot_client.publish((m_content_topic + "/get").c_str(), "");
  };

  // Creates device shadow topics.
  // General format: $aws/things/<ThingName>/shadow/name/<ShadowName>/<Action>
  const char* getDeviceShadowTopic(std::string action) {
    return ("$aws/things/" + std::string(m_thing_name) + "/shadow/name/" + std::string(m_device_shadow_name) + action).c_str();
  };

};
#endif
