/*
 *  Simple MQTT client
 */


#include <ESP8266WiFi.h>
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>


const char* ssid     = "IoT2018";
const char* password = "00000000";

const char* host = "wifitest.adafruit.com";

// START MQTT SECTION
//#include will be needed depending on your Ethernet shield type

#define MQTT_MAX_PACKET_SIZE 100
#define SIZE 100

#define MQTT_PORT 1883
#define PUBLISH_TOPIC "iot-2/evt/status/fmt/json"
#define SUBSCRIBE_TOPIC "iot-2/cmd/+/fmt/json"
#define AUTHMETHOD "use-token-auth"

int ledPin = 13;

#define CLIENT_ID "d:45zrtf:TestDeviceType:TestDeviceType1"
#define MS_PROXY "45zrtf.messaging.internetofthings.ibmcloud.com"
#define AUTHTOKEN "qwertyuiop"

// Update these with values suitable for your network.

byte mac[] = { 0x98, 0x4F, 0xEE, 0x01, 0x9D, 0x99 };

//EthernetClient c;
//IPStack ipstack(c);

WiFiClient wclient;
IPStack ipstack(wclient);
MQTT::Client<IPStack, Countdown, 100, 1> mclient = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

void messageArrived(MQTT::MessageData& md);

String deviceEvent;

// END MQTT SECTION
void setup() {
  Serial.begin(230400);
  delay(100);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  serverSetup();
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  mqttHandler();
  clientConnectLoop();
}

void mqttHandler() {
  int rc = -1;
  
  if (!mclient.isConnected()) {
    Serial.print("Connecting using Registered mode with clientid : ");
    Serial.print(CLIENT_ID);
    Serial.println("\tto MQTT Broker : ");
    Serial.print(MS_PROXY);
    Serial.print("\ton topic : ");
    Serial.println(PUBLISH_TOPIC);

    rc = ipstack.connect(MS_PROXY, MQTT_PORT);

    MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
    options.MQTTVersion = 3;
    options.clientID.cstring = CLIENT_ID;
    options.username.cstring = AUTHMETHOD;
    options.password.cstring = AUTHTOKEN;
    options.keepAliveInterval = 60;
    rc = 0;

    while (rc == 0)
    {
        rc = mclient.connect(options);
        Serial.println(rc);
    }

    //unsubscribe the topic, if it had subscribed it before.

//    mclient.unsubscribe(SUBSCRIBE_TOPIC);

    //Try to subscribe for commands
    if ((rc = mclient.subscribe(SUBSCRIBE_TOPIC, MQTT::QOS0, messageArrived)) != 0) {
            Serial.print("Subscribe failed with return code : ");
            Serial.println(rc);
    } else {
          Serial.println("Subscribed\n");
    }
    Serial.println("Subscription tried......");
    Serial.println("Connected successfully\n");
//    Serial.println("Temperature(in C)\tDevice Event (JSON)");
//    Serial.println("____________________________________________________________________________");
  }

  MQTT::Message message;
  message.qos = MQTT::QOS0; 
  message.retained = false;

  char *json/*[56]*/ = "{\"d\":{\"myName\":\"Arduino-1\",\"temperature\":\"53.33\"}}";
//
//  Serial.print("\t");
//  Serial.print(tempValue);
//  Serial.print(33.33);
//  Serial.print("\t\t");
//  Serial.println(json);
//  message.payload = json; 
//  message.payloadlen = strlen(json);
//  rc = mclient.publish(PUBLISH_TOPIC, message);
//  if (rc != 0) {
//    Serial.print("Message publish failed with return code : ");
//    Serial.println(rc);
//  }
  mclient.yield(1000);
}

void messageArrived(MQTT::MessageData& md) {
  Serial.print("\nMessage Received\t");
    MQTT::Message &message = md.message;
    int topicLen = strlen(md.topicName.lenstring.data) + 1;

    char * topic = md.topicName.lenstring.data;
    topic[topicLen] = '\0';
    
    int payloadLen = message.payloadlen + 1;

    char * payload = (char*)message.payload;
    payload[payloadLen] = '\0';
    
    String topicStr = topic;
    String payloadStr = payload;
    
    //Command topic: iot-2/cmd/blink/fmt/json

    if(strstr(topic, "/cmd/blink") != NULL) {
      Serial.print("Command IS Supported : ");
      Serial.print(payload);
      Serial.println("\t.....\n");
      

      //Blink
      for(int i = 0 ; i < 2 ; i++ ) {
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
      }

    } else {
      Serial.println("Command Not Supported:");            
    }
}

double getTemp(void) {
    return 33.33;
}
