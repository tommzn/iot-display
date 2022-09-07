# iot-display (WIP)
Manage contents on e-Ink displays powered by an ESP32 via AWS IOT.

# Setup
Before compiling and uploading this sketch to your ESP32 you've to create all required resources.
Have a look at aws to get more details about it.

## Add Certs, Keys and Endpoint
After your created all required AWS IOT resource your've to adjust folloeing settings.
- Add AWS Root CA certificate to AWS_CERT_CA in esp32/iot-display/aws_iot_certs.h
- Add client certificate to AWS_CERT_CRT in esp32/iot-display/aws_iot_certs.h
- Add private key to AWS_CERT_PRIVATE in esp32/iot-display/aws_iot_certs.h
- Adjust AWS IOT endpoint corresponding to your AWS region at m_iot_endpoint in esp32/iot-display/aws_iot_client.h

