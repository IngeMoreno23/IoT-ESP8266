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
  initMQTT();
}

void loop() {
  // put your main code here, to run repeatedly:
  checkWifiConnection();
  checkMQTTConnection();

  mqttClient.publish("Hola", "Yo soy Mateo");

  handleMQTT();
  delay(1000);
}