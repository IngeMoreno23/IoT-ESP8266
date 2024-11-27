import paho.mqtt.client as paho
import mysql.connector
import mysql.connector.errorcode as errorcode
import threading
import json
import time

MQTT_BROKER_ADRESS: str = "192.168.35.142"
MQTT_BROKER_PORT: int = 1883
DOWNSTREAM_TOPIC: str = "ESP8266_to_Python/topic"
UPSTREAM_TOPIC: str = "Python_to_ESP8266/topic"

apprun = True

# Diccionario con cliente y sus variables
localData = {
    "ESP8266Client_1": {"humidity": None, "temperature": None, "distance": None},
    "ESP8266Client_2": {"humidity": None, "temperature": None, "distance": None},
    "ESP8266Client_3": {"humidity": None, "temperature": None, "distance": None},
    "ESP8266Client_4": {"humidity": None, "temperature": None, "distance": None},
    "ESP8266Client_5": {"humidity": None, "temperature": None, "distance": None},
    "ESP8266Client_6": {"humidity": None, "temperature": None, "distance": None}
}

# MySQL Database configuration
DATABASE_HOST = "localhost"
DATABASE_USER = "root"
DATABASE_PASSWORD = "HolaSoyVegana_23"

DATABASE_NAME = "test_data_002"
CLIENT_TABLE = "Cliente"
SENSOR_TABLE = "Sensor"
ENTRY_TABLE = "Registro"

# MySQL setup
def setup_database():
    try:
        connection = mysql.connector.connect(host=DATABASE_HOST, user=DATABASE_USER, password=DATABASE_PASSWORD)
        cursor = connection.cursor()
        
        # Create database if not exists
        try:
            cursor.execute(f"CREATE DATABASE {DATABASE_NAME}")
            print(f"Database '{DATABASE_NAME}' created successfully.")
        except mysql.connector.Error as err:
            if err.errno == errorcode.ER_DB_CREATE_EXISTS:
                print(f"Database '{DATABASE_NAME}' already exists.")
            else:
                print(err.msg)
        connection.database = DATABASE_NAME

        # Crear tabla Cliente si no existe
        cliente_table_query = f"""
        CREATE TABLE IF NOT EXISTS {CLIENT_TABLE} (
            idCliente INT PRIMARY KEY AUTO_INCREMENT,
            nombre VARCHAR(255) NOT NULL
        );
        """
        cursor.execute(cliente_table_query)
        print(f"Tabla '{CLIENT_TABLE}' lista.")

        # Crear tabla Sensor si no existe
        sensor_table_query = f"""
        CREATE TABLE IF NOT EXISTS {SENSOR_TABLE} (
            idSensor INT PRIMARY KEY AUTO_INCREMENT,
            tipo VARCHAR(50) NOT NULL
        );
        """
        cursor.execute(sensor_table_query)
        print(f"Tabla '{SENSOR_TABLE}' lista.")

        # Crear tabla Registro si no existe
        registro_table_query = f"""
        CREATE TABLE IF NOT EXISTS {ENTRY_TABLE} (
            idRegistro INT PRIMARY KEY AUTO_INCREMENT,
            tiempo DATETIME NOT NULL,
            dato FLOAT, 
            idCliente INT NOT NULL,
            idSensor INT NOT NULL,
            FOREIGN KEY (idCliente) REFERENCES {CLIENT_TABLE}(idCliente),
            FOREIGN KEY (idSensor) REFERENCES {SENSOR_TABLE}(idSensor)
        );
        """ # dato puede ser NULL
        cursor.execute(registro_table_query)
        print(f"Tabla '{ENTRY_TABLE}' lista.")

        # Verificar o insertar clientes
        for client_name in localData.keys():
            cursor.execute("SELECT idCliente FROM Cliente WHERE nombre = %s", (client_name,))
            result = cursor.fetchone()
            if result is None:  # Si no existe, lo insertamos
                cursor.execute("INSERT INTO Cliente (nombre) VALUES (%s)", (client_name,))
                print(f"Cliente '{client_name}' insertado.")

        # Insertar sensores (solo claves de la primera fila)
        sensors = list(localData["ESP8266Client_1"].keys())
        for sensor in sensors:
            cursor.execute("SELECT idSensor FROM Sensor WHERE tipo = %s", (sensor,))
            result = cursor.fetchone()
            if result is None:  # Si no existe, lo insertamos
                cursor.execute("INSERT INTO Sensor (tipo) VALUES (%s)", (sensor,))
                print(f"Sensor '{sensor}' insertado.")

        # Confirmar cambios
        connection.commit()

    except mysql.connector.Error as err:
        print(f"Error: {err}")
    finally:
        if cursor:
            cursor.close()
        if connection:
            connection.close()

def insert_in_database():
    try:
        connection = mysql.connector.connect(host=DATABASE_HOST, user=DATABASE_USER, password=DATABASE_PASSWORD, database=DATABASE_NAME)
        cursor = connection.cursor()
        

        for client_name, sensors in localData.items():
            # Paso 1: Verificar si el cliente existe, si no existe, insertarlo
            cursor.execute("SELECT idCliente FROM Cliente WHERE nombre = %s", (client_name,))
            result = cursor.fetchone()
            
            if result is None:
                # Insertar nuevo cliente si no existe
                cursor.execute("INSERT INTO Cliente (nombre) VALUES (%s)", (client_name,))
                connection.commit()

            # Obtener el idCliente de la base de datos
            cursor.execute("SELECT idCliente FROM Cliente WHERE nombre = %s", (client_name,))
            client_id = cursor.fetchone()[0]

            # Paso 2: Insertar o verificar existencia de los sensores
            for sensor_type, sensor_value in sensors.items():
                # Verificar si el sensor ya existe en la base de datos
                cursor.execute("SELECT idSensor FROM Sensor WHERE tipo = %s", (sensor_type,))
                sensor_result = cursor.fetchone()

                if sensor_result is None:
                    # Insertar nuevo sensor si no existe
                    cursor.execute("INSERT INTO Sensor (tipo) VALUES (%s)", (sensor_type,))
                    connection.commit()

                # Obtener el idSensor
                cursor.execute("SELECT idSensor FROM Sensor WHERE tipo = %s", (sensor_type,))
                sensor_id = cursor.fetchone()[0]

                # Paso 3: Insertar el registro del sensor con el valor actual
                cursor.execute("""
                    INSERT INTO Registro (idCliente, idSensor, dato, tiempo)
                    VALUES (%s, %s, %s, NOW())
                """, (client_id, sensor_id, sensor_value))
                connection.commit()

        print("Inserted complete data into database.")
        # Clear data after insertion
        for client_id in localData:
            for sensor in localData[client_id]:
                localData[client_id][sensor] = None
    except mysql.connector.Error as err:
        print(f"Database Error: {err}")
    finally:
        if cursor:
            cursor.close()
        if connection:
            connection.close()

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
        if data["MQTT_CLIENT_NAME"] != None:
            localData[data["MQTT_CLIENT_NAME"]]["humidity"]    = data["humidity"]
            localData[data["MQTT_CLIENT_NAME"]]["temperature"] = data["temperature"]
            localData[data["MQTT_CLIENT_NAME"]]["distance"]    = data["distance"]


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

    current_fan_state: str = "0"

    current_state: str = "green"
    last_state_change: float = 0
    sensor_threshold: int = 10
    traffic_light_yellow_time: float = 2
    traffic_light_red_time: float = 5
    traffic_light_current_multiplier: int = 1
    traffic_light_max_multiplier: int = 3
    traffic_light_cooldown: float = 5 # seconds
    traffic_light_response = {"traffic_light": current_state, "fan_state": current_fan_state}

    global localData
    global apprun
    while apprun:
        client.loop()
        print("Local data: ", localData)

        # Logica del semaforo
        if current_state == "green":
            if current_time - last_state_change >= traffic_light_cooldown and any((data.get("distance") is not None and int(data["distance"]) < sensor_threshold and int(data["distance"]) != 0) for client_, data in localData.items()):
                current_state = "yellow"
                traffic_light_response["traffic_light"] = current_state
                last_state_change = current_time
        elif current_state == "yellow":
            if current_time - last_state_change >= traffic_light_yellow_time:
                current_state = "red"
                traffic_light_response["traffic_light"] = current_state
                last_state_change = current_time
        else: # Its red
            
            if current_time - last_state_change <= traffic_light_red_time*traffic_light_current_multiplier and traffic_light_current_multiplier <= traffic_light_max_multiplier:
                if current_time - last_state_change >= traffic_light_red_time*traffic_light_current_multiplier-3 and any((data.get("distance") is not None and int(data["distance"]) < sensor_threshold and int(data["distance"]) != 0) for client_, data in localData.items()):
                    # Una persona esta cerca en los ultimos segundo y todavia no se alcanza el tiempo maximo}
                    traffic_light_current_multiplier += 1
            else: # time up
                current_state = "green"
                traffic_light_response["traffic_light"] = current_state
                last_state_change = current_time
                traffic_light_current_multiplier = 1

        # L+ogica del abanico
        if any((data["temperature"] is not None and data["temperature"] > 28) for client_, data in localData.items()):
            current_fan_state = "1"
            traffic_light_response["fan_state"] = current_fan_state
        else :
            current_fan_state = "0"
            traffic_light_response["fan_state"] = current_fan_state

        # Lógica del publicadoe
        if current_time - previous_time < time_interval:
            current_time = time.time()
        else:
            previous_time = current_time
            #print("Publicamos ", traffic_light_response)
            #print("current_time: ", current_time)
            client.publish(UPSTREAM_TOPIC, json.dumps(traffic_light_response), qos=1)
            insert_in_database()


def main():
    setup_database()
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