void initMQTT(){
    mqttClient.setServer(MQTT_BROKER_ADRESS, MQTT_PORT);
    subscribeMQTT();
    mqttClient.setCallback(callback);
}

void handleMQTT(){
    checkMQTTConnection();
    mqttClient.loop();
}