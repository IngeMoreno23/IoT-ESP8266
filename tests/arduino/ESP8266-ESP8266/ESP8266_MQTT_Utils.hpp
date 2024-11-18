void initMQTT(PubSubClient& myClient, const char* clientName){
    myClient.setServer(MQTT_BROKER_ADRESS, MQTT_PORT);
    if(clientName != MQTT_CLIENT_NAME_1)
      subscribeMQTT(myClient);
    myClient.setCallback(callback);
}

void handleMQTT(PubSubClient& myClient, const char* clientName){
    checkMQTTConnection(myClient, clientName);
    myClient.loop();
}