# Arduino ESP8266MOD script

Este apartado contendrá el contenido general del projecto en el microcontrolador `main` y como hacer un **setup** para su uso en este projecto

## Contenido

Arduino IDE no tiene la el board de NodeMCU por defecto, esto se arregla agregando la siguiente dirección en `file > preferences > Additional board manager URLs`:

`https://arduino.esp8266.com/stable/package_esp8266com_index.json`

### config.h

Aquí se enlistan viariables para conectar al microcontrolador a internet a los microcontroladores.

```
const char* ssid     = "network_name";
const char* password = "password";
const char* hostname = "ESP8266_X";
```

### ESP8266_Utils.hpp

Antes de realizar una conexión al Broker MQTT, debemos de conectarnos a una red WiFi. La librería `#include <ESP8266WiFi.h>` nos facilita una instancia nombrada `WiFi` de la clase `ESP8266WiFiClass` que encapsula todas la necesidades para conectar el microcontrolador a una red WiFi.

En este header se inicializará la conexión a internet con los datos `ssid` y `password` del documento `config.h` con el siguiente codigo:

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
```

Esta función será llamada cada vez que se pierda la conexión WiFi, lo cual es posible detectar con la linea de código:

```
WiFi.status() != WL_CONNECTED
```

Por lo que cada vez que el programa realice un loop, correremos esta función `connectWifi()` con el siguiente método, renovando la conexión:

```
void checkWifiConnection(){
    if (WiFi.status() != WL_CONNECTED){
        Serial.println("Conexión perdida, intentando reconectar...");
        connectWifi();
    }
}
```

**Nota:**

Es posible utilizar

```
WiFi.setAutoReconnect(true);
WiFi.persistent(true);
```

en el  `void setup()`, que permiten a la conexión reconectarse automaticamente si hay una desconexión y guardar las credenciales `ssid` y `password` en este caso, pero **no garantiza que el programa se ejecute con una conexión WiFi continua**, por lo que para este proyecto se realiza la reconexión manualmente.

### MQTT.hpp

Este header contendrá 