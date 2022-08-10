/**
 * Wifi conneciton handler implementation.
 */

#include "WiFiConnection.h"
#include "WiFi.h"
#include "Arduino.h"

// Create a new conneciton handler and pass credentials and settings.
WiFiConnection::WiFiConnection(char* ssid, char* password, int max_connect_attemps) {
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

  Serial.print("Try to connect to WiFi ");
  Serial.print(m_ssid);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < m_connect_attemps) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Success");
    return true;
  } else {
    Serial.println("Failed");
    return false;
  }
}

// Returns true if there's a connection to defined WiFi.
bool WiFiConnection::connected() {
  return WiFi.status() == WL_CONNECTED;
}

// Disconnect from Wifi and switch to mode WIFI_OFF.
void WiFiConnection::disconnect() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.print("WiFi disconnected!");
}

// Returns MAC address of used device.
String WiFiConnection::getMacAddress() {
  return WiFi.macAddress();
}

// Returns current IP address from WiFi connection.
IPAddress WiFiConnection::getIPAddress() {
  return WiFi.localIP();
}
