#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "#insert_wifi_ssid_here"
#define WIFI_PASS "#insert_wifi_pass_here"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "#insert_name_here"
#define MQTT_PASS "#insert_pass_here"

const int leftForward = 4;
const int leftBackward = 5;
const int rightForward = 14;
const int rightBackward = 12;

int led = LED_BUILTIN;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/onof");
Adafruit_MQTT_Publish LightsStatus = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/status");


void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);


  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  digitalWrite(LED_BUILTIN, LOW);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }

  Serial.println("OK!");

  //Subscribe to the onoff topic
  mqtt.subscribe(&onoff);

  pinMode(led, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(led, LOW);
  pinMode(leftForward , OUTPUT);
  pinMode(16,OUTPUT);
  pinMode(leftBackward , OUTPUT);
  pinMode(rightForward , OUTPUT);
  pinMode(rightBackward , OUTPUT);
  digitalWrite(16,HIGH);

}

void loop()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &onoff)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) onoff.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) onoff.lastread, "OFF"))
      {
        //active low logic
        digitalWrite(led, HIGH);
        LightsStatus.publish("ON");
      }
      else if (!strcmp((char*) onoff.lastread, "ON"))
      {
        digitalWrite(led, LOW);
        LightsStatus.publish("OFF");

      }
       else if (!strcmp((char*) onoff.lastread, "BLINK"))
      {
        while((subscription != mqtt.readSubscription(5000)))     {
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led, LOW);
         delay(500);
        LightsStatus.publish("BLINK");
      }
      }
     else if (!strcmp((char*) onoff.lastread, "FORWARD")){
              while((subscription != mqtt.readSubscription(5000)))     {

      digitalWrite(leftForward , LOW);
      digitalWrite(leftBackward , HIGH);
      digitalWrite(rightForward , LOW);
      digitalWrite(rightBackward , HIGH);
              LightsStatus.publish("FORWARD");

     }
     }
         else if (!strcmp((char*) onoff.lastread, "BACKWARD")){
                  while((subscription != mqtt.readSubscription(5000)))     {

      digitalWrite(leftForward , HIGH);
      digitalWrite(leftBackward , LOW);
      digitalWrite(rightForward , HIGH);
      digitalWrite(rightBackward , LOW);
              LightsStatus.publish("BACKWARD");
                  }

     }
        else if (!strcmp((char*) onoff.lastread, "RIGHT")){
                  while((subscription != mqtt.readSubscription(5000)))     {

        
      digitalWrite(leftForward , LOW);
      digitalWrite(leftBackward , LOW);
      digitalWrite(rightForward , HIGH);
      digitalWrite(rightBackward , LOW);
              LightsStatus.publish("RIGHT");
        }

     }
        else if (!strcmp((char*) onoff.lastread, "LEFT")){
                  while((subscription != mqtt.readSubscription(5000)))     {

      digitalWrite(leftForward , HIGH);
      digitalWrite(leftBackward , LOW);
      digitalWrite(rightForward , LOW);
      digitalWrite(rightBackward , LOW);
              LightsStatus.publish("LEFT");
                  }
     }
      }
      else
      {
        LightsStatus.publish("ERROR");
      }
    
  }
  //  if (!mqtt.ping())
  //  {
  //    mqtt.disconnect();
  //  }
}


void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
