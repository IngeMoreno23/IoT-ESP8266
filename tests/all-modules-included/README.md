# All modules included Project

> [Po-Po-Po-Po-Ponele Guaracha](https://www.youtube.com/watch?v=cfUSa1j1uow)

Preparence, y tomen asiento, porque en 1 solo protoboard y ESP8266, es posible tener todo el sistema.

## Contenido Final

Este script consistirá en una prueba solitaria en la que se valide la funcionalidad de cada módulo, los cuales serán los siguientes:

+ Toma de datos de arduino
+ Publicación de datos ESP8266 - Python
+ Análisis de datos.
+ Inserción de datos en MySQL
+ Publicación de datos Python - ESP8266

### config.h

Comenzando con la conexión a WiFi, se necesitarán estas variables para realizar una conexión desde las **Estaciones**, las cuales serán en nuestro caso: ESP8266, mobile hotspot (datos del celular) y el broker MQTT.

```
const char* ssid     = "network_name";
const char* password = "password";
const char* hostname = "ESP8266_X";
```

**Nota:**

Actualmente no usamos el hostname, pero si tienes problemas identificando de donde proviene una conexión entre varios ESP8266, puedes utilizarlo dentro de los **payloads** en paquetes de información para identificarlos. Nosotros utilizaremos otra variable para denominar cada cliente que estará en `MQTT.hpp`

### ESP8266_Utils.hpp

Muy probablemente este codigo no se modifique en la totalidad del proyecto

```
void connectWifi(){
    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password);

    // Esperar a que se conecte
    Serial.print("Conectando");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println();

        Serial.print("Conectado, dirección IP: ");
        Serial.println(WiFi.localIP());
}

void checkWifiConnection(){
    if (WiFi.status() != WL_CONNECTED){
        Serial.println("Conexión perdida, intentando reconectar...");
        connectWifi();
    }
}
```



## Setup - Pruebalo tu mismo

En este apartado podrás configurar el programa para correrlo en tu microcontrolador.