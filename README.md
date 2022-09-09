# IOT-Display
Manage contents on eInk displays powered by an ESP32 via AWS IOT topics.

This is an example implementation for an ESP32 which powers an eInk display. It uses AWS IOT to manage settings, e.g. deep sleep time, via device shadows
and listens on a spefific topic for new contents to update displayed values if required. 

# Content Management
To get new contents the device subscribes to 'iotdisplay/things/<ThingName>/contents' topic. It will not constantly listen for new content, rather ESP32 deep sleep mode will be used to periodically wake up and process new content. After wake up it triggers a content update with message send to topic 'iotdisplay/things/<ThingName>/contents/get'.

## Content Format
Values or texts which should be shown on a display are exchanged in JSON format between a backend and this device. Payload send to 'iotdisplay/things/<ThingName>/contents' topic uses following format.
```json
{
    "content_hash": "2647553784567356",
    "items": [{
        "text": "Hi there!",
        "position": {
            "x": 100,
            "y": 100
        },
        ...
    }]
}
``` 
### Content Hash
Content hash can be used to determine if something has been changed and a display update is required. The device will persist last content hash for future checks. 

### Items
List of values/text, together with a position, which should be shown on a display.

## Settings Management
Device settings are managed via AWS IOT device shadow service. It's used to manage number of seconds the ESP32 should stay in deep sleep mode. With this approach you can change settings for a device, even if it's not online at the moment. The device will fetch new settings after is wakes up from deep sleep.

## Content Manager (Backend)
There's an example implementation of a content manager which sends new contents to 'iotdisplay/things/<ThingName>/contents' topic at [IOT-Display ContentManager](https://github.com/tommzn/iotdisplay-contentmanager). It subscribes to 'iotdisplay/things/<ThingName>/contents/get' topic and simply returns thing name along with current timestamp on 'iotdisplay/things/<ThingName>/contents' topic.

## Logging
With an active AWS IOT connection the device sends logs to 'iotdisplay/logs' topic. You can subscribe to this topic to process this logs. [IOT-Display LogForwarder](https://github.com/tommzn/iotdisplay-logforwarder) provides an example implementation to collect this logs and forward them to Logz.io.

# Setup
This project provides an Arduino sketch you can upload to your ESP32. See [ESP32 Sketch](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display).

## Create AWS Resources
Before compiling and uploading this sketch to your ESP32 you've to create all required resources.
Have a look at [AWS IOT Setup](https://github.com/tommzn/iot-display/tree/main/aws) to get more details about it.

## Add Certs, Keys and Endpoint
After you've created all required AWS IOT resources please adjust folloeing settings.
- Add AWS Root CA certificate to AWS_CERT_CA in [aws_iot_certs.h](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display/aws_iot_certs.h)
- Add client certificate to AWS_CERT_CRT in [aws_iot_certs.h](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display/aws_iot_certs.h)
- Add private key to AWS_CERT_PRIVATE in [aws_iot_certs.h](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display/aws_iot_certs.h)
- Adjust AWS IOT endpoint corresponding to your AWS region at AWS_IOT_ENDPOINT in [aws_iot_settings.h](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display/aws_iot_settings.h)
- Update AWS IOT thing name at AWS_IOT_THING_NAME in [aws_iot_settings.h](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display/aws_iot_settings.h)
- Provide your Wifi credentials in [wifi_credentials.h](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display/wifi_credentials.h)

## Assign Content Manager
You'll need something that publishes new contents to 'iotdisplay/things/<ThingName>/contents' topic. 

# Links
- [ESP32 Sketch](https://github.com/tommzn/iot-display/tree/main/esp32/iot-display)
- [AWS IOT Setup](https://github.com/tommzn/iot-display/tree/main/aws)
- [IOT-Display ContentManager](https://github.com/tommzn/iotdisplay-contentmanager)
- [IOT-Display LogForwarder](https://github.com/tommzn/iotdisplay-logforwarder)

