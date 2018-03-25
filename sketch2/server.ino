/*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Starts WiFi server with fix IP and listens
 *  Receives and sends messages to the client
 *  Communicates: wifi_client_01.ino
 */
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


WiFiServer server(80);                    

IPAddress ip(192, 168, 0, 80);            // IP address of the server
IPAddress gateway(192,168,0,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network

int speakerPin = 14;

static int count = 0;

void serverSetup() 
{
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
/*  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network */
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
//  Serial.print("SSID: "); Serial.println(WiFi.SSID());
//  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
//  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());
//  pinMode(ledPin, OUTPUT);
}

void clientConnectLoop() 
{
  WiFiClient client = server.available();
  if (client) 
  {
    if (client.connected()) 
    {
      digitalWrite(ledPin, LOW);  // to show the communication only (inverted logic)
      Serial.println(".");
      client.println("send operating data ^_^\r"); // get temp, motion, proximity, etc.
      String response = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: "); Serial.println(response);
      client.println("thank :D\r"); // sends the answer to the client

      actOnConditions(response);

//      String request = client.readStringUntil('\r');
      client.flush();
      digitalWrite(ledPin, HIGH);
    }
    client.stop();                // tarminates the connection with the client
  }
}


void actOnConditions(String conditions)
{
    char conditionsArray[150]; 
    conditions.toCharArray(conditionsArray, 150);
    
    StaticJsonBuffer<512> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(conditionsArray);
    JsonObject& d = root["d"];

    if(!root.success())
    {
      Serial.println("parseObject() failed");
    }

    double temp = d["temperature"];
    int motion = d["motion"];

    Serial.println(String("T: ") + temp + String("; M: ") + motion);

    if(temp > 28.0)
    {
      Serial.println("Too hot!");
      playSoundsHot();
    }

    if(motion == 1 && count >= 2)
    {
      count = 0;
      Serial.println("Gesture activated");
      // playSoundsMotion();
    }
    else
    {
      count++;
    }
}

void playSoundsHot()
{
  int numTones = 10;
  int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};

  for(int i = 0; i < numTones; i++)
  {
    tone(speakerPin, tones[i]);
    delay(200);
  }
  noTone(speakerPin);
  
  
}

void playSoundsMotion()
{
  int numTones = 10;
  int tones[] = {361, 377, 394, 411, 430, 449, 470, 492, 515, 540};

  for(int i = 0; i < numTones; i++)
  {
    tone(speakerPin, tones[i]);
    delay(25);
  }
  noTone(speakerPin);
  
}

