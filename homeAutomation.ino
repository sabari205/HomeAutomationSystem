#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#define DHTTYPE DHT11   //defining the type of the temperature sensor

#define dht_dpin 0    //the pin to which the sensor is connected
DHT temp_sensor(dht_dpin, DHTTYPE); 
 
//pins for the relay module
#define L1 D2
#define L2 D6   

#define ssid "sabari"       //ssid of the wifi network to be connected
#define pwd "vinohari"  //password for the wifi network

#define AIO_SERVER "io.adafruit.com"  //link for the cloud platform
#define AIO_SERVERPORT 1883     //port for the cloud platform
#define AIO_USERNAME "sabari205" //username for the account created
#define AIO_KEY "aio_xVbp98OYVlDVJQEZeX73qLh4Qftk" //the key generated for the adafruit account
WiFiClient client;  //create an instance for the client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);    //creating the instance for the client using the account details
 //creating objects for subscription and publishing data
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light1");
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light2");
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

int x = 0;
void MQTT_connect(); //function declaration for establishing the connection to the server

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
//establishing a wifi connection
  Serial.print(“Wifi connection");
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) { //checks for the status of the wifi
    delay(2000);
    Serial.print(“*");
  }
//Setup MQTT subscription for the lights feed.
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);
//prints the ssid and ip address of the wifi network connected
  Serial.println();
  Serial.println("connected to ");
  Serial.print(WiFi.SSID());
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

}

void loop() {
  MQTT_connect(); //establishing a connection to the MQTT server
  Adafruit_MQTT_Subscribe *subscription;
  // If light 1 details are changed
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &Light1) {
      Serial.print(F(“Light 1 details : "));
      Serial.println((char *)Light1.lastread);
      if (strncmp((char *)Light1.lastread, "ON",2) == 0) {
        digitalWrite(L1, LOW); 
      }
      if (strncmp((char *)Light1.lastread, "OFF",3) == 0) {
        digitalWrite(L1, HIGH); 
      }
    }
    // If light2 details are changed
    if (subscription == &Light2) {
      Serial.print(F(“Light 2 details : "));
      Serial.println((char *)Light2.lastread);
      if (strncmp((char *)Light2.lastread, "ON",2) == 0) {
        digitalWrite(L2, LOW); 
      }
      if (strncmp((char *)Light2.lastread, "OFF",3) == 0) {
        digitalWrite(L2, HIGH); 
      }
    }
  }
  if(x % 2000 == 0){
    float h = temp_sensor.readHumidity(); //read the humidity value
    float t = temp_sensor.readTemperature(); //read the temperature value
    Serial.print(F(“\nSend temperature "));
    Serial.print(t);
  // Publishing temperature value to the feed
   if (! Temperature.publish(t)) {
      Serial.println(F(“Temperature publish failed !”));
    } else {
      Serial.println(F("OK!"));
    } 
    Serial.print(F(“\nSend humidity"));
    Serial.print(h);

    // Publishing humidity value to the feed
    if (! Humidity.publish(h)) {
      Serial.println(F(“Humidity publish failed !”));
    } else {
      Serial.println(F(“Humidity publish successful !"));
    } 
  }
  x++;
}

//function definition for establishing the connection to the server
void MQTT_connect() {
  int8_t connector;

  if (mqtt.connected()) { //checks the status for the connection to the server
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t count = 3;  //the number of retries to attempt incase of a failure
  
  while ((connector = mqtt.connect()) != 0) {
    Serial.println(“Retrying Messsage Queue connection !”);
    //prints the error message to the serial monitor
    Serial.println(mqtt.connectErrorString(connector)); 
    mqtt.disconnect();
    delay(5000); 
    count = count - 1;
    if (! count) {
      while (1);
    }
  }
  Serial.println(“Message Queue Connection established");
  
}

