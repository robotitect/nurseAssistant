/*
 *  Simple MQTT client
 */


#include <ESP8266WiFi.h>
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <string.h>

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
int A = A0;
int motion = 16;
//int echo=12;
//int trig=14;

#define CLIENT_ID "d:45zrtf:TestDeviceType:TestDeviceType2"
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
  pinMode (A, INPUT);
  pinMode (motion, INPUT);
  //pinMode(echo, INPUT);
  //pinMode(trig, OUTPUT);
  Serial.begin(115200);
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
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  mqttHandler();
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

    mclient.unsubscribe(SUBSCRIBE_TOPIC);
    //Try to subscribe for commands
    if ((rc = mclient.subscribe(SUBSCRIBE_TOPIC, MQTT::QOS0, messageArrived)) != 0) {
            Serial.print("Subscribe failed with return code : ");
            Serial.println(rc);
    } else {
          Serial.println("Subscribed\n");
    }
    Serial.println("Subscription tried......");
    Serial.println("Connected successfully\n");
    Serial.println("Temperature(in C)\tDevice Event (JSON)");
    Serial.println("____________________________________________________________________________");

    
  }

  MQTT::Message message;
  message.qos = MQTT::QOS0; 
  message.retained = false;

  //char json[56] = "{\"d\":{\"myName\":\"Arduino-1\",\"temperature\":\"33.33\"}";
//  Serial.println(createMessage());
  String jsonStr = createString();
  char json[200];
  jsonStr.toCharArray(json, 200);
//  Serial.println(json);

//  char buffer[10];
//  float tempValue = getTemp();
//  dtostrf(tempValue,1,2, &json[43]);
//  dtostrf(getTemp(),1,2, buffer);

//  json[48] = '}';
//  json[49] = '}';
//  json[50] = '\0';
  Serial.print("\t");
//  Serial.print(tempValue);
  Serial.print(33.33);
  Serial.print("\t\t");
  Serial.println(json);
  message.payload = json; 
  message.payloadlen = strlen(json);
  rc = mclient.publish(PUBLISH_TOPIC, message);
  if (rc != 0) {
    Serial.print("Message publish failed with return code : ");
    Serial.println(rc);
  }

  // communicate this stuff to the other huzzah guy (server)
  serverConnectLoop(jsonStr);
    
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

double getTemp() {
  int reading = analogRead(A);
  double temp = (reading - 500)/10.0;
  return temp;
}
/*
double dsens(){
  double t=0;
  digitalWrite (trig, HIGH);
  delay (10);
  digitalWrite (trig, LOW);
  t=pulseIn(echo, HIGH);
  return (t/58.3);
}*/

boolean checkMotion()
{
//  char toReturn[10] = "";
  if(digitalRead(motion) == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}


char *createMessage()
{
  char toReturn[150] = ""; 
  // "{\"d\":{\"myName\":\"Arduino-1\",\"temperature\":\"33.33\"}"
  strcpy(toReturn, "{\"d\":{\"myName\":\"Arduino-1\",\"temperature\":");
  char t[8];
//  toReturn[30]=t;
  Serial.println(String("Temperature is") + getTemp());
  sprintf(t, "%f", float(getTemp()));   
  t[7] = 0; 
//  dtostrf(getTemp(), '5', '2', t);
  strcat(toReturn, t);
//  strcat(toReturn, ",\"motion\":");
//  if(checkMotion())
//  {
//    strcat(toReturn, "\"True\"");
//  }
//  else
//  {
//    strcat(toReturn, "\"False\"");
//  }
  strcat(toReturn, "}}");
  // Serial.println(toReturn);
  return toReturn;
}

String createString()
{
  String toReturn = "{\"d\":{\"myName\":\"Arduino-1\",\"temperature\":";
  toReturn += getTemp();
  toReturn += ",\"motion\":";
  toReturn += checkMotion();
  //toReturn += ",\"distance\":";
  //toReturn += dsens();
  toReturn += "}}";
  return toReturn;
}

