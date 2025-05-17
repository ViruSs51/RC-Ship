#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define CE_PIN 9
#define CSN_PIN 10
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 7
#define ENA 6 
#define ENB 5
#define SERVO A0

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "hghq}";

Servo servo;

int j1x, j1y, j2x, j2y;
char command[8];

void initRadio() {
  radio.begin();
  radio.setAddressWidth(5); 
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(108);
  radio.setRetries(5, 15); 
  radio.startListening();
}

void initMotors() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0); 
}

void control() {
  if (strcmp(command, "00001111") == 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, j2y);
    analogWrite(ENB, j2y); 
  } else if (strcmp(command, "11110000") == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, j2y);
    analogWrite(ENB, j2y); 
  } else if (strcmp(command, "10011110") == 0 || strcmp(command, "01110001") == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, j1x);
    analogWrite(ENB, j1x); 
  } else if (strcmp(command, "01011111") == 0 || strcmp(command, "11111010") == 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, j1x);
    analogWrite(ENB, j1x); 
  }

  servo.write(j2x);
}

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO);

  initRadio();
  initMotors();
}

void loop() {
  if (radio.available()) {
    char payload[50];
    radio.read(&payload, sizeof(payload));

    Serial.print("Received: ");
    Serial.println(payload);

    sscanf(payload, "%s ,%d,%d,%d", command, &j1x, &j2x, &j2y);
    command[8] = '\0';
  }

  control();
}
