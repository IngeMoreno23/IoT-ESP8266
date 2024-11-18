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

  connectWifi();
  initMQTT(mqttClientUpstreamer, MQTT_CLIENT_NAME_1);
  initMQTT(mqttClientDownstreamer, MQTT_CLIENT_NAME_2);
}

void loop() {
  
  checkWifiConnection();
  checkMQTTConnection(mqttClientUpstreamer, MQTT_CLIENT_NAME_1);
  checkMQTTConnection(mqttClientDownstreamer, MQTT_CLIENT_NAME_2);

  if(currentTime - previousTime < timeInterval){
    currentTime = millis();
  } else {
    previousTime = currentTime;

    // Publish logic
    simulateDHT();
    simulateUltrasonicDistance();

    if(humidity != NAN && temperature != NAN && distance != NAN){
      doc["MQTT_CLIENT_NAME"] = MQTT_CLIENT_NAME_1; // Quien es el UPSTREAMER
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

    mqttClientUpstreamer.publish(UPSTREAM_TOPIC, charUpstreamPayload, true);  // Retained: true
    printActuators();
  }
    
  // Recibimos el mensaje del broker y reconectamos
  handleMQTT(mqttClientUpstreamer, MQTT_CLIENT_NAME_1);
  handleMQTT(mqttClientDownstreamer, MQTT_CLIENT_NAME_2);
  
}