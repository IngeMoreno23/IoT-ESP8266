const char* MQTT_BROKER_ADRESS = "192.168.54.142";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME = "ESP8266Client_1";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void subscribeMQTT(){
    // mqttClient.subscribe("Hola");
}

void connectMQTT(){
    Serial.println("Conectando a MQTT...");
    while (!mqttClient.connected())
    {
        if (mqttClient.connect(MQTT_CLIENT_NAME))
        {
            Serial.println("Conectado a MQTT");
        }
        else
        {
            Serial.print("Error al conectar a MQTT, rc=");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
}

void checkMQTTConnection(){
    if (!mqttClient.connected()){
        Serial.println("Conexión perdida, intentando reconectar...");
        connectMQTT();
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