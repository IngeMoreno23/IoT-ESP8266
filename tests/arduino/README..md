# Arduino ESP8266MOD test

La documentación del proyecto actual se ubicará en `IOT-ESP8226\arduino`, aqui se realizará una prueba simuada de conección entre 2 ESP8266 y entre un ESP8266 y Python, así como el test de evaluación de todos los casos posibles en `tests\arduino\main`.

## ESP8266 - ESP8266

Comenzando por una conexión entre broker MQTT y arduino.
Iniciaremos 2 clientes con nombres distintos, a cada uno se le concederá el mismo callback y ambos publicaran y se suscribiran al mismo tópico.

De esta manera, podemos verificar que la conexión entre arduino y broker sea correcta y podrémos probar funcionalidades como **retained** que es parte de los parámetros de PubSubClient `publish()`.

### Principales cambios

#### initMQTT()

La inicialización del cliente ahora será definido de esta manera en `MQTT.hpp`, con un WiFiClient distinto para cada PubSubClient ya que si se usa el mismo podría causar errores de conexión:

```
WiFiClient espClient1;
WiFiClient espClient2;
PubSubClient mqttClient1(espClient1);
PubSubClient mqttClient2(espClient2);
```

Y el `void setup()` del main file llamara la inicialización con cada cliente:

```
initMQTT(mqttClient1, MQTT_CLIENT_NAME_1);
initMQTT(mqttClient2, MQTT_CLIENT_NAME_2);
```

Ahora todas las funciones recibiran un PubSubClient por referencia para que la reconeción se realice en esa instancia, y podrémos reutilizar el código para los 2 clientes, el codigo resultante en el `void loop()` contendrá lo siguiente para que:

1. Se veriifique la conexión de los clientes y se resuscriban al tópico si hay una desconexión.
2. Se publique desde Client1 al tópico `"test/topic"`.
3. Realicen un `client.loop()` para no perder la conexión y escuchar las publicaciones.

```
checkMQTTConnection(mqttClient1, MQTT_CLIENT_NAME_1);
checkMQTTConnection(mqttClient2, MQTT_CLIENT_NAME_2);

mqttClient1.publish("test/topic", "Hola, yo soy Client1 y me dirijo a mis suscriptores", true);

handleMQTT(mqttClient1, MQTT_CLIENT_NAME_1);
handleMQTT(mqttClient2, MQTT_CLIENT_NAME_2);
```

## ESP8266 - Python



## Python - ESP8266



## test\arduino\main

