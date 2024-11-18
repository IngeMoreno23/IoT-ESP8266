void initMQTT(PubSubClient& myClient, const char* clientName){
    myClient.setServer(MQTT_BROKER_ADRESS, MQTT_PORT);
    subscribeMQTT(myClient, clientName);
    myClient.setCallback(callback);
}

void handleMQTT(PubSubClient& myClient, const char* clientName){
    checkMQTTConnection(myClient, clientName);
    myClient.loop();
}