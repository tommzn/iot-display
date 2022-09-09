/**
 * Wifi conneciton handler implementation.
 */

#include "WiFiConnection.h"
#include "WiFi.h"

// Create a new conneciton handler and pass credentials and settings.
WiFiConnection::WiFiConnection(const char* ssid, const char* password, uint8_t max_connect_attemps) {
  m_connect_attemps = max_connect_attemps;
  m_ssid = ssid;
  m_password = password;
}

// Will try to connet to defined Wifi with given credentials.
 // Returns true on success or false in case of errors.
 // Writes status messages to Serial.
bool WiFiConnection::connect() {

  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(m_ssid, m_password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < m_connect_attemps) {
    delay(500);
    retries++;
  }

  return WiFi.status() == WL_CONNECTED;
}

// Returns true if there's a connection to defined WiFi.
bool WiFiConnection::connected() {
  return WiFi.status() == WL_CONNECTED;
}

// Disconnect from Wifi and switch to mode WIFI_OFF.
void WiFiConnection::disconnect() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

// Returns MAC address of used device.
String WiFiConnection::getMacAddress() {
  return WiFi.macAddress();
}

// Returns current IP address from WiFi connection.
IPAddress WiFiConnection::getIPAddress() {
  return WiFi.localIP();
}
