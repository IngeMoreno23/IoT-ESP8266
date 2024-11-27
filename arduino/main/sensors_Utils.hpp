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


// Actuadores Luces de Tráfico y Ventilador
const int redPin = D4;
const int yellowPin = D5;
const int greenPin = D6;

const int fanPin = D8;
char trafficLightState[10];
char fanState[2];
int errorFlashing = 0;

// Declaraciones de las funciones para leer los sensores
void getDHT();
void getUltrasonicDistance();

void simulateDHT();
void simulateUltrasonicDistance();

void updateTrafficLigth();
void updateFan();
void simulateActuators();

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


void updateTrafficLigth(){
  if (strcmp(trafficLightState, "red") == 0){
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
  } else if (strcmp(trafficLightState, "yellow") == 0){
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin, LOW);
  } else if (strcmp(trafficLightState, "green") == 0){
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH);
  } else {
    if(errorFlashing >= 0){
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, LOW);
      errorFlashing++;
    } else {
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, HIGH);
      errorFlashing--;  
    }
    
  }
}

void updateFan(){
  if (strcmp(fanState, "1") == 0){
    Serial.print("Hola me estoy prendiendo: ");
    Serial.println(fanState);
    digitalWrite(fanPin, HIGH);
  } else {
    Serial.print("Hola me estoy apagando: ");
    Serial.println(fanState);
    digitalWrite(fanPin, LOW);
  }
}

void simulateActuators(){
  int randomState = random(0, 5);
  if (randomState % 2 == 0){
    strcpy(fanState, "0");
  } else {
    strcpy(fanState, "1");
  }
  if (randomState % 3 == 0){
    strcpy(trafficLightState, "Red");
  } else if (randomState % 3 == 1){
    strcpy(trafficLightState, "Yellow");
  } else {
    strcpy(trafficLightState, "Green");
  }
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
  Serial.print("Ventiladore: ");
  Serial.println(fanState);
  Serial.print("----------\n");
}