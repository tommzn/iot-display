#include "Arduino.h"

#include "aws_iot_client.h"
#include "timer.h"

// Include AWS IOT devices certificates
#include "aws_iot_certs.h"

// Connects to AWS IOT endpoint specified by AWS_IOT_ENDPOINT using thing name from AWS_IOT_THING_NAME.
// Since it's a encryted connection certs from AWS_CERT_CA, AWS_CERT_PRIVATE and AWS_CERT_CRT are used.
// After successful connect client subsribes to content topic AWS_IOT_CONTENT_TOPIC and 
// shadow topics AWS_IOT_SHADOW_GET_ACCEPTED_TOPIC, AWS_IOT_SHADOW_REJECT_GET_TOPIC and AWS_IOT_SHADOW_REJECT_UPDATE_TOPIC.
bool AwsIotClient::connect() {
  
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  m_secure_client.setCACert(AWS_CERT_CA);
  m_secure_client.setCertificate(AWS_CERT_CRT);
  m_secure_client.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to AWS IOT
  m_iot_client.begin(m_iot_endpoint, 8883, m_secure_client);
  // Extend default timeout because data collection may take some seconds.
  m_iot_client.setKeepAlive(m_connection_keep_alive);
    
  Serial.print("Try to connect to AWS IOT Endpoint: ");
  Serial.println(m_iot_endpoint);
  uint8_t retries = 0;
  while (!m_iot_client.connect(m_thing_name) && retries < m_max_connect_attemps) {
    retries++;
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  
  if (m_iot_client.connected()) {
    Serial.println("Connected to AWS IOT!");
    //m_iot_client.subscribe(m_content_topic);
    //triggerContentGet();
    //m_iot_client.subscribe(getDeviceShadowTopic("/get/accepted"));
    //m_iot_client.subscribe(getDeviceShadowTopic("/get/rejected"));
    //m_iot_client.subscribe(getDeviceShadowTopic("/update/rejected"));
    return true;
  } else {
    Serial.println("Unable to connected to AWS IOT!");
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
