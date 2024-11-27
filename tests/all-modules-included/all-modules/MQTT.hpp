const char* MQTT_BROKER_ADRESS = "192.168.214.142";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME_1 = "ESP8266Client_1";
const char* MQTT_CLIENT_NAME_2 = "ESP8266Client_2";

const char* DOWNSTREAM_TOPIC = "Python_to_ESP8266/topic";
const char* UPSTREAM_TOPIC = "ESP8266_to_Python/topic";

WiFiClient espClientUpstreamer;     // Quien publicará
WiFiClient espClientDownstreamer;   // Quien suscribirá

PubSubClient mqttClientUpstreamer(espClientUpstreamer);
PubSubClient mqttClientDownstreamer(espClientDownstreamer);

void subscribeMQTT(PubSubClient& myClient, const char* clientName){
    if(clientName != MQTT_CLIENT_NAME_2) // ELEGIR EL CLIENTE DOWNSTREAMER PARA QUE SOLO EL ESCUCHE
        return;
    if(myClient.subscribe(DOWNSTREAM_TOPIC)){
        Serial.print("Suscripción exitosa a: ");
        Serial.println(DOWNSTREAM_TOPIC);
    } else {
        Serial.print("Error al suscribirse a: ");
        Serial.println(DOWNSTREAM_TOPIC);
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
            subscribeMQTT(myClient, clientName);
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

    JsonDocument doc;
    deserializeJson(doc, message);
    // Do a user check
    strcpy(trafficLightState, doc["traffic_light"]);
}