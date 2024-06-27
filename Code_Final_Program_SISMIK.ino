#include <Wire.h>
#include <BH1750.h>

#define trigPin1 9
#define echoPin1 8

#define trigPin2 11
#define echoPin2 10

#define ledPin1 6

int check = 0; 
int vehicle = 0;
bool lampu_hidup = false;
BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  lightMeter.begin();
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT);
  pinMode(ledPin1, OUTPUT);
  TCCR1A = 0;
  TCCR1B = 0;
}

ISR(TIMER1_COMPB_vect) {
  if(check < 300 && check > 0){
    TCNT1   = 0;  
    OCR1B   = 62500;
    check++;
  }
  else if(check == 5){
    analogWrite(ledPin1, 25.5);
    vehicle = 0;
    check = 0;
  }
}

int sensor_intensitas(){
  uint16_t lux = lightMeter.readLightLevel();
  return lux;
}

int sensor_ultrasonik(int a, int b){
  long duration;
  int distance;

  digitalWrite(a, LOW);
  delayMicroseconds(2);
  digitalWrite(a, HIGH);
  delayMicroseconds(10);
  digitalWrite(a, LOW);
  duration = pulseIn(b, HIGH);
  distance = (duration / 2) / 29.4;
  return distance;
}

void mobilLewatSensor1() {
  check = 0;
  vehicle++;
  if (vehicle != 0) {
    analogWrite(ledPin1, 255);
  }
}

void mobilLewatSensor2() {
  check = 1;
  vehicle--;
  if (vehicle < 0){
    vehicle=0;
  }
  TCCR1B |= (1 << CS12) | (1 << CS10); 
  TCNT1   = 0;  
  OCR1B   = 62500;
  TIMSK1 |= (1 << OCIE1B);
  TIFR1  |= (1 << OCF1B);
}

void loop() {
  int value_ultra1 = sensor_ultrasonik(trigPin1, echoPin1);
  int value_ultra2 = sensor_ultrasonik(trigPin2, echoPin2);
  int value_cahaya = sensor_intensitas();

  Serial.print("S. Intensitas : ");
  Serial.print(value_cahaya);
  Serial.println(" lux");

  Serial.print("S. Ultra1 : ");
  Serial.print(value_ultra1);
  Serial.println(" cm");

  Serial.print("S. Ultra 2: ");
  Serial.print(value_ultra2);
  Serial.println(" cm");

  Serial.print("mobil : ");
  Serial.println(vehicle);

  if(value_cahaya < 200 ){
    if(!lampu_hidup){
      analogWrite(ledPin1, 25.5);
      lampu_hidup = true;
    }
    if (value_ultra1 < 6) { 
      mobilLewatSensor1();
    }
    if (value_ultra2 < 6) { 
      mobilLewatSensor2();
    }
  } else {
    analogWrite(ledPin1,0);
    lampu_hidup = false;
  }
}