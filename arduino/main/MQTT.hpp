const char* MQTT_BROKER_ADRESS = "192.168.54.227";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME = "ESP8266Client_3";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

