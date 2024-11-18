import paho.mqtt.client as paho
import threading
import json
import time

MQTT_BROKER_ADRESS: str = "192.168.54.142"
MQTT_BROKER_PORT: int = 1883
DOWNSTREAM_TOPIC: str = "ESP8266_to_Python/topic"
UPSTREAM_TOPIC: str = "Python_to_ESP8266/topic"

apprun = True

# Diccionario con cliente y sus variables
localData = {
    "ESP8266Client_1": {"humidity": 0, "temperature": 0, "distance": 0}
}

def on_connect(client, userdata, flags, rc, properties=None):
    if rc != 0:
        print("Error en la conexión, rc= {rc}")
    else:
        print("Conectado al broker MQTT")

def on_message(client, userdata, message):
    # print("Mensaje recibido =", str(message.payload.decode("utf-8")))
    # print("Mensaje topic =", message.topic)
    # print("Mensaje qos =", message.qos)
    # print("Mensaje retain flag =", message.retain)

    # Guardar datos en el diccionario
    global localData
    if message.topic == DOWNSTREAM_TOPIC:
        data = json.loads(str(message.payload.decode("utf-8")))
        if data["MQTT_CLIENT_NAME"] == "ESP8266Client_1":
            localData["ESP8266Client_1"]["humidity"]    = data["humidity"]
            localData["ESP8266Client_1"]["temperature"] = data["temperature"]
            localData["ESP8266Client_1"]["distance"]    = data["distance"]


def on_publish(client, userdata, mid, rc, properties=None):
    print("Mensaje publicado con mid =", mid)

def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscripción exitosa")


def listener(client):
    print("Iniciando listener thread")
    client.loop_forever()

def traffic_light_publisher(client):
    current_time: float = 0
    previous_time: float = 0
    time_interval: float = 1

    last_state_change: float = 0
    current_state: str = "green"
    sensor_threshold: int = 10
    traffic_light_yellow_time: float = 2
    traffic_light_red_time: float = 5
    traffic_light_current_multiplier: int = 1
    traffic_light_max_multiplier: int = 3
    traffic_light_cooldown: float = 10 # seconds
    traffic_light_response = {"MQTT_CLIENT_NAME": "ESP8266Client_2", "traffic_light": current_state}

    global localData
    global apprun
    while apprun:
        client.loop()
        print("Local data: ", localData)
        if current_state == "green":
            if current_time - last_state_change >= traffic_light_cooldown and any(int(data["distance"]) < sensor_threshold for client_, data in localData.items()):
                current_state = "yellow"
                traffic_light_response["traffic_light"] = current_state
                last_state_change = current_time
        elif current_state == "yellow":
            if current_time - last_state_change >= traffic_light_yellow_time:
                current_state = "red"
                traffic_light_response["traffic_light"] = current_state
                last_state_change = current_time
        else: # Its red
            
            if current_time - last_state_change <= traffic_light_red_time*traffic_light_current_multiplier:
                if current_time - last_state_change >= traffic_light_red_time*traffic_light_current_multiplier-3 and traffic_light_max_multiplier < 3:
                    # Una persona esta cerca en los ultimos segundo y todavia no se alcanza el tiempo maximo}
                    traffic_light_current_multiplier += 1
            else: # time up
                current_state = "green"
                traffic_light_response["traffic_light"] = current_state
                last_state_change = current_time


        if current_time - previous_time < time_interval:
            current_time = time.time()
        else:
            previous_time = current_time
            print("Publicamos ", traffic_light_response)
            client.publish(UPSTREAM_TOPIC, json.dumps(traffic_light_response), qos=1)


def main():
    py_client_Upstreamer = paho.Client(callback_api_version= paho.CallbackAPIVersion.VERSION2,
                                       client_id= "py_client_Upstreamer")
    py_client_Downstreamer = paho.Client(callback_api_version= paho.CallbackAPIVersion.VERSION2,
                                       client_id= "py_client_Downstreamer")

    py_client_Upstreamer.on_connect = on_connect
    py_client_Upstreamer.on_message = on_message
    py_client_Upstreamer.on_publish = on_publish
    py_client_Upstreamer.on_subscribe = on_subscribe

    py_client_Downstreamer.on_connect = on_connect
    py_client_Downstreamer.on_message = on_message
    py_client_Downstreamer.on_publish = on_publish
    py_client_Downstreamer.on_subscribe = on_subscribe

    py_client_Upstreamer.connect(MQTT_BROKER_ADRESS, MQTT_BROKER_PORT)
    py_client_Downstreamer.connect(MQTT_BROKER_ADRESS, MQTT_BROKER_PORT)

    py_client_Downstreamer.subscribe(DOWNSTREAM_TOPIC, 1)


    try:
        print("Presione Ctrl+C para salir")
        thread_listener = threading.Thread(target=listener, args=(py_client_Downstreamer,))
        thread_publisher = threading.Thread(target=traffic_light_publisher, args=(py_client_Upstreamer,))
        thread_listener.start()
        thread_publisher.start()
        while True:
            ...

    except KeyboardInterrupt:
        print("Desconectando del broker MQTT")
        global apprun
        apprun = False
        py_client_Upstreamer.disconnect()
        py_client_Downstreamer.disconnect()
        thread_listener.join()
        thread_publisher.join()
        print("Saliendo...")




if __name__ == "__main__":
    main()