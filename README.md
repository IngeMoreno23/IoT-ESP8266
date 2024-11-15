# IoT-ESP8266
Implementation of a traffic light with an ESP8266 microcontroller, Python and MySQL.

## Descripción general

La conexión entre ESP8266 al script de python se realizara con la publicación y subscripción a 2 topicos, estos dependeran de la categoria en los que se encuentren.

1. **upstream** (datos enviados de ESP8266 a Python)
    + `sensorUltrasonico`
    + `sensorTemperatura`
    + `sensorHumedad`
2. **downstream** (datos enviados de Python a ESP8266)
    + `actuadorSemaforo`
    + `actuadorVentilador`


## Formato JSON
Cada dato se subira en alguno de estos tópicos en un JSON.

El tópico **upstream** tendrá un JSON descrito de esta manera:

```
{
    "client_id" : "ESP8266_X",
    "sensor" : "sensor_name",
    "data" : "data"
}
```

Esto nos ayudará para que al momento de identificar en que variable guardar los datos en el Python y tenerlo estructurado para realizar queries a MySQL más facilmente.

Para el tópico **downstream**, realmente también utilizaremos el mismo JSON, el cual no es necesario para el semáforo ya que ambos deberán de apagarse y prenderse al mismo tiempo, pero lo importante es el ventilador, que si es único por semáforo.

**Nota:**

El problema del semáforo podría ser arreglado probablemente subiendo `"client_id"` como una lista, pero como es el único sistema que lo requiere, solamente utilizaremos 2 publicaciones, una para cada microcontrolador, sin necesidad de realizar otro método de **callback** para este sistema.

## Clientes MQTT

**Python** solo tendrá un cliente de lectura, el cual identificará los clientes y el sensor del que proviene el dato y lo guardará en variables locales.

De lado contrario, los **ESP8266** tendrán 1 cliente cada uno y la manera en la que se identificarán será igualando el `"client_id"` con el nombre de su cliente.