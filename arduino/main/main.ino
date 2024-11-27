#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "sensors_Utils.hpp"

#include "config.h"
#include "ESP8266_Utils.hpp"
#include "MQTT.hpp"
#include "ESP8266_MQTT_Utils.hpp"


JsonDocument doc;

long currentTime = 0;
long previousTime = 0;
const long timeInterval = 1000; // timeInterval in ms for publish.

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando...");

  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(fanPin, OUTPUT);


  connectWifi();
  initMQTT();
}

void loop() {
  
  checkWifiConnection();
  checkMQTTConnection();

  if(currentTime - previousTime < timeInterval){
    currentTime = millis();
  } else {
    previousTime = currentTime;

    // Publish logic
    getDHT();
    getUltrasonicDistance();

    if(humidity != NAN && temperature != NAN && distance != NAN){
      doc["MQTT_CLIENT_NAME"] = MQTT_CLIENT_NAME; // Quien es el UPSTREAMER
      doc["humidity"] = humidity;
      doc["temperature"] = temperature;
      doc["distance"] = distance;
    }

    // Convertimos el objeto a un string y char*
    String upstreamPayload;
    serializeJson(doc, upstreamPayload);

    char charUpstreamPayload[upstreamPayload.length() + 1];  // Reserva espacio en la pila para el arreglo
    upstreamPayload.toCharArray(charUpstreamPayload, upstreamPayload.length() + 1);  // Copia el String al arreglo charArray

    // Subimos el mensaje al broker

    mqttClient.publish(UPSTREAM_TOPIC, charUpstreamPayload, true);  // Retained: true
    updateTrafficLigth();
    updateFan();
  }
    
  // Recibimos el mensaje del broker y reconectamos
  handleMQTT();
}