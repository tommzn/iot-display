#include "Arduino.h"

#include "aws_iot_client.h"
#include "timer.h"

// Include AWS IOT devices certificates
#include "aws_iot_certs.h"

// Begin adds certificates (AWS_CERT_CA, AWS_CERT_CRT) and private key (AWS_CERT_PRIVATE) 
// to secure client and inits the iot client.
void AwsIotClient::begin() {
  
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  m_secure_client.setCACert(AWS_CERT_CA);
  m_secure_client.setCertificate(AWS_CERT_CRT);
  m_secure_client.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to AWS IOT
  m_iot_client.begin(m_iot_endpoint, 8883, m_secure_client);
  // Extend default timeout because data collection may take some seconds.
  m_iot_client.setKeepAlive(m_connection_keep_alive);

}

// Connects to AWS IOT endpoint specified by AWS_IOT_ENDPOINT using thing name from AWS_IOT_THING_NAME.
bool AwsIotClient::connect() {

  Serial.println();
  Serial.print("Try to connect to AWS IOT Endpoint: ");
  Serial.println(m_iot_endpoint);
  
  uint8_t retries = 0;
  while (!m_iot_client.connect(m_thing_name) && retries < m_max_connect_attemps) {
    retries++;
    Serial.print(".");
    delay(500);
  }
  
  if (m_iot_client.connected()) {
    return true;
  } else {
    logError();
    return false;
  }
}

// Disconnects from AWS IOT with some delay defined by m_disconnect_delay.
// During this delay MQTT will flush remaining messages.
bool AwsIotClient::disconnect() {

  Timer timer;
  timer.start(m_disconnect_delay);
  while (!timer.isExpired()) {
    loop();
    delay(100);
  }
  return m_iot_client.disconnect();
}

// Subsribes to content and device shadow topics.
void AwsIotClient::subsribe() {
    
  if (!m_iot_client.subscribe(m_content_topic.c_str())) {
    logError();
    return;
  }

  // Subscribe to device shadow topics.
  m_iot_client.subscribe((m_shadow_topic + "/get/accepted").c_str());
  m_iot_client.subscribe((m_shadow_topic + "/get/rejected").c_str());
  m_iot_client.subscribe((m_shadow_topic + "/update/rejected").c_str());
}

// Publishs current settings to device shadow topic for update.
void AwsIotClient::updateDeviceShadow(uint32_t sleep_time) {

  StaticJsonDocument<200> doc;
  JsonObject deviceState = doc.createNestedObject("state");
  JsonObject reportedState = deviceState.createNestedObject("reported");
  reportedState["deep_sleep_seconds"] = sleep_time;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  m_iot_client.publish((m_shadow_topic + "/update").c_str(), jsonBuffer);
}
