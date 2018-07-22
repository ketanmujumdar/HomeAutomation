
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       ""
#define WLAN_PASS       ""

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""
#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/hdmi1");
Adafruit_MQTT_Subscribe changehdmi2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/hdmi2");
Adafruit_MQTT_Subscribe tv = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/tv");

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.
int reset=0;

void MQTT_connect();
void hdmi1();
void turnOffTv();
void hdmi2();

void setup() {
  reset=1;
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&changehdmi2);
  mqtt.subscribe(&tv);
}

 /**
  * This function would switch to HDMI1
  */
 void hdmi1(){
    Serial.println("Clicked Input");
  irsend.sendNEC(0x20DFD02F, 32);
    delay(3000);
  Serial.println("Clicked Arrow Down");  
  irsend.sendNEC(0x20DF827D, 32);
    delay(2000);
  Serial.println("Clicked Arrow Down");
  irsend.sendNEC(0x20DF827D, 32);
    delay(2000);
  Serial.println("Clicked OK");  
  irsend.sendNEC(0x20DF22DD, 32);
  delay(2000);
  Serial.println("Click Back");
  irsend.sendNEC(0x20DF14EB, 32);
 }

  /**
  * This function would switch to HDMI1
  */
 void hdmi2(){
    Serial.println("Clicked Input");
  irsend.sendNEC(0x20DFD02F, 32);
    delay(3000);
  Serial.println("Clicked Arrow Down");  
  irsend.sendNEC(0x20DF827D, 32);
    delay(2000);
  Serial.println("Clicked Arrow Down");
  irsend.sendNEC(0x20DF827D, 32);
    delay(2000);
  Serial.println("Clicked Arrow Down");
  irsend.sendNEC(0x20DF827D, 32);
    delay(2000);
  Serial.println("Clicked OK");  
  irsend.sendNEC(0x20DF22DD, 32);
  delay(2000);
  Serial.println("Click Back");
  irsend.sendNEC(0x20DF14EB, 32);
 }

/**
 * This function would turn off the TV
 */
void turnOffTv(){
   irsend.sendNEC(0x20DF10EF, 32);
    delay(3000);
}

/**
 * This will turn off the TV
 */
void turnOnTv(){
   irsend.sendNEC(0x20DF10EF, 32);
    delay(3000);
}

void loop() {

    // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton && reset != 1) {
       hdmi1();
       reset=1;
    }
    if (subscription == &changehdmi2 && reset != 1) {
       hdmi2();
       reset=1;
    }
    if (subscription == &tv && reset != 1) {
       Serial.println((char *)tv.lastread);
       turnOffTv(); 
       reset=1;
    }
  }

  reset=0;
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
 
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
