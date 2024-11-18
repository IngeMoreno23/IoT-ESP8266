// Sensor de Humedad y Temperatura (DHT)
#include <DHT.h>
#define DHTPIN D7      
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

float humidity;
float temperature;

const int trigPin = D1;
const int echoPin = D2;

// Constantes de conversión
const float soundSpeed = 0.034; // Velocidad del sonido en cm/µs
const int conversionFactor = 2; // Para dividir el tiempo de ida y vuelta

// Variable para almacenar la distancia medida
long duration;
float distance;


// Variables para actuadores
char trafficLightState[10];
int fanState;


// Declaraciones de las funciones para leer los sensores
void getDHT();
void getUltrasonicDistance();

void simulateDHT();
void simulateUltrasonicDistance();

void printSensors();
void printActuator();

void getDHT(){
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void getUltrasonicDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Enviar un pulso de 10 microsegundos al TRIG
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Leer el tiempo de retorno del pulso
  duration = pulseIn(echoPin, HIGH);

  // Calcular la distancia en centímetros
  distance = duration * soundSpeed / conversionFactor;
}

void simulateDHT(){
  humidity = (float)random(50000, 55000) / 1000.0;
  temperature = (float)random(26000, 28000) / 1000.0;
}

void simulateUltrasonicDistance(){
  distance = (float)random(40000, 50000) / 1000.0;
}

void printSensors(){
  Serial.print("----------\n");
  Serial.print("Sensores:\n");
  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print("%\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("----------\n");
}

void printActuators(){
  Serial.print("----------\n");
  Serial.print("Actuadores:\n");
  Serial.print("Semáforo: ");
  Serial.println(trafficLightState);
  Serial.print("----------\n");
}