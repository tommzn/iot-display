/**
 * Wifi conneciton handler.
 * Contains credentials and handles WiFi connect/disconnet.
 */
#include <WiFi.h>

#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

class WiFiConnection {
public:

  WiFiConnection(char* ssid, char* password, int max_connect_attemps);

  // Will try to connet to defined Wifi with given credentials.
  // Returns true on success or false in case of errors.
  bool connect();

  // Returns true if there's a connection to defined WiFi.
  bool connected();
  
  // Disconnect from defined WiFi.
  void disconnect();

  // Returns MAC address of used device.
  String getMacAddress();

  // Returns current IP address from WiFi connection.
  IPAddress getIPAddress();

private:

  // Max. WiFi connect attemps. If exceeded connect() will reutrn with false.
  int m_connect_attemps;

  // WiFi SSID
  char* m_ssid;

  // WiFi Password
  char* m_password;
  
};
#endif 
