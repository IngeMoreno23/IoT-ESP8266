#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"
#include "ESP8266_Utils.hpp"
#include "MQTT.hpp"
#include "ESP8266_MQTT_Utils.hpp"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Inicializando...");

  connectWifi();
  initMQTT(mqttClient1, MQTT_CLIENT_NAME_1);
  initMQTT(mqttClient2, MQTT_CLIENT_NAME_2);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkWifiConnection();
  checkMQTTConnection(mqttClient1, MQTT_CLIENT_NAME_1);
  checkMQTTConnection(mqttClient2, MQTT_CLIENT_NAME_2);

  mqttClient1.publish("test/topic", "Hola, yo soy Client1 y me dirijo a mis suscriptores", true);

  handleMQTT(mqttClient1, MQTT_CLIENT_NAME_1);
  handleMQTT(mqttClient2, MQTT_CLIENT_NAME_2);

  delay(1000);
}