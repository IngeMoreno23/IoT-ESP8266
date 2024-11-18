const char* MQTT_BROKER_ADRESS = "192.168.54.142";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME_1 = "ESP8266Client_1";
const char* MQTT_CLIENT_NAME_2 = "ESP8266Client_2";

WiFiClient espClient1;
WiFiClient espClient2;
PubSubClient mqttClient1(espClient1);
PubSubClient mqttClient2(espClient2);

void subscribeMQTT(PubSubClient& myClient){
    if(myClient.subscribe("test/topic")){
        Serial.println("Suscripción exitosa a 'test/topic'");
    } else {
        Serial.println("Error al suscribirse a 'test/topic'");
    }
}

void connectMQTT(PubSubClient& myClient, const char* clientName){
    Serial.print("Conectando a MQTT[");
    Serial.print(clientName);
    Serial.println("]...");
    while (!myClient.connected())
    {
        if (myClient.connect(clientName))
        {
            Serial.print("Conectado a MQTT[");
            Serial.print(clientName);
            Serial.println("]");
            if(clientName != MQTT_CLIENT_NAME_1)
                subscribeMQTT(myClient);
        }
        else
        {
            Serial.print("Error al conectar a MQTT[");
            Serial.print(clientName);
            Serial.print("], rc=");
            Serial.println(myClient.state());
            delay(5000);
        }
    }
}

void checkMQTTConnection(PubSubClient& myClient, const char* clientName){
    if (!myClient.connected()){
        Serial.print("Conexión perdida [" );
        Serial.print(clientName);
        Serial.print("], intentando reconectar...");
        connectMQTT(myClient, clientName);
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensaje recibido [");
    Serial.print(topic);
    Serial.print("]: ");
    char message[length+1];
    for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    Serial.print(message);
    Serial.println();
}