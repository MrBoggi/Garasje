#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "config.h"

DHT dht{DHT_PIN, DHT_TYPE};      // Initiate DHT library
HX711 scale;                     // Initiate HX711 library
WiFiClient wifiClient;           // Initiate WiFi library
PubSubClient client(wifiClient); // Initiate PubSubClient library


//Definere pinner for bruk mot garasjeport
int PortÅpen = 2; // Dersom denne er høy er garasjeport Åpen
int PortStengt = 1; // Dersom denne er høy er garasjeport Stengt

void setup()
{
  Serial.begin(74880);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting...");

  while (WiFi.status() != WL_CONNECTED)
  { // Wait till Wifi connected
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); // Print IP address

  client.setServer(MQTT_SERVER, 1883);              // Set MQTT server and port number
  client.setCallback(callback);                     // Set callback address, this is used for remote tare

  Serial.println("Start DHT library");
  dht.begin();

  // Definere pinner for input
  pinMode(PortStengt, INPUT);
  pinMode(PortÅpen, INPUT);
}

void loop()
{
      // Ensure we are still connected to MQTT Topics
    if (!client.connected())
    {
        reconnect();
    }
    // Leser status for de to endebryterne på garaasjeporten
    Open = digitalRead(PortÅpen);
    Closed = digitalRead(PortStengt);
    if (Open)
    {
        PortStatus = 'Åpen'
    }
    else if (Closed)
    {
        PortStatus = 'Stengt'
    }
    else
    {
        PortStatus = 'Halvåpen/Ukjent'
    }
    
    client.publish(PORT_TOPIC, PortStatus);         // Publish weight to the STATE topic

    client.loop();      // MQTT task loop

    // Reading values from the DHT sensor
    float humidity = dht.readHumidity();       // Read humidity
    float temperature = dht.readTemperature(); // Read temperature

    // Check if we recieved a number from dht libraray
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Error while parse values to numbers from dht");
    }

    client.publish(TEMPERATURE_TOPIC, String(temperature).c_str()); // Publish temperature to the temperature value topic
    client.publish(HUMIDITY_TOPIC, String(humidity).c_str());       // Publish humidity to the humidity value topic
}

void reconnect()
{
  while (!client.connected())
  { // Loop until connected to MQTT server
    Serial.print("Attempting MQTT connection...");
    if (client.connect(HOSTNAME, mqtt_username, mqtt_password))
    { //Connect to MQTT server
      Serial.println("connected");
      client.publish(AVAILABILITY_TOPIC, "online"); // Once connected, publish online to the availability topic
      client.subscribe(TARE_TOPIC);                 //Subscribe to tare topic for remote tare
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); // Will attempt connection again in 5 seconds
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  if (strcmp(topic, TARE_TOPIC) == 0)
  {
    Serial.println("starting tare...");
    scale.wait_ready();
    scale.set_scale();
    scale.tare(); //Reset scale to zero
    Serial.println("Scale reset to zero");
  }
}