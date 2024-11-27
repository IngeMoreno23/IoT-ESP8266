const char* MQTT_BROKER_ADRESS = "192.168.214.142";
const uint16_t MQTT_PORT = 1883;
String MQTT_CLIENT_BASE = "ESP8266Client_";
String mqttClientName = (MQTT_CLIENT_BASE) + MICROCONTROLLER_NUMBER;
const char* MQTT_CLIENT_NAME = mqttClientName.c_str();


const char* DOWNSTREAM_TOPIC = "Python_to_ESP8266/topic";
const char* UPSTREAM_TOPIC = "ESP8266_to_Python/topic";

WiFiClient espClient;     // Quien publicará

PubSubClient mqttClient(espClient);

void subscribeMQTT(){

    if(mqttClient.subscribe(DOWNSTREAM_TOPIC)){
        Serial.print("Suscripción exitosa a: ");
        Serial.println(DOWNSTREAM_TOPIC);
    } else {
        Serial.print("Error al suscribirse a: ");
        Serial.println(DOWNSTREAM_TOPIC);
    }
}

void connectMQTT(){
    Serial.print("Conectando a MQTT[");
    Serial.print(MQTT_CLIENT_NAME);
    Serial.println("]...");
    while (!mqttClient.connected())
    {
        if (mqttClient.connect(MQTT_CLIENT_NAME))
        {
            Serial.print("Conectado a MQTT[");
            Serial.print(MQTT_CLIENT_NAME);
            Serial.println("]");
            subscribeMQTT();
        }
        else
        {
            Serial.print("Error al conectar a MQTT[");
            Serial.print(MQTT_CLIENT_NAME);
            Serial.print("], rc=");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
}

void checkMQTTConnection(){
    if (!mqttClient.connected()){
        Serial.print("Conexión perdida [" );
        Serial.print(MQTT_CLIENT_NAME);
        Serial.print("], intentando reconectar...");
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

    JsonDocument doc;
    deserializeJson(doc, message);
    // Do a user check
    strcpy(trafficLightState, doc["traffic_light"]);
}