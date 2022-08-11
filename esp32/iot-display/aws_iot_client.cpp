#include "aws_iot_client.h"

// Include AWS IOT devices certificates
#include "aws_iot_certs.h"

bool AwsIotClient::connect() {
  
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  m_secure_client.setCACert(AWS_CERT_CA);
  m_secure_client.setCertificate(AWS_CERT_CRT);
  m_secure_client.setPrivateKey(AWS_CERT_PRIVATE);

  uint32_t retries = 0;
  while (!m_iot_client.connect(m_thing_name) && retries < m_max_connect_attemps) {
    retries++;
    delay(500);
  }

  m_iot_client.begin(m_iot_endpoint, 8883, m_secure_client);
  m_iot_client.setKeepAlive(m_connection_keep_alive); 
  
  return m_iot_client.connected();
}

bool AwsIotClient::disconnect() {
  
  unsigned long endAt = millis() + m_disconnect_delay;
  while (endAt > millis()) {
    m_iot_client.loop();
  }
  m_iot_client.disconnect();
}

void AwsIotClient::publishLog(const char* message) {
  m_iot_client.publish(m_log_topic, message);
}
