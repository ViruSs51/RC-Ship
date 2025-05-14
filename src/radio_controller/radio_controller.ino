#include <SPI.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10
#define J1Y A0
#define J1X A1
#define J2X A2

const byte address[10] = "101010010";

int j1x, j1y;
int j2x;

RF24 radio(CE_PIN, CSN_PIN);

void initRadio() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void readJoiticks(int &x1, int &y1, int &x2) {
  x1 = analogRead(J1X);
  y1 = analogRead(J1Y);
  x2 = analogRead(J2X);
}

void setup() {
  Serial.begin(9600);
  pinMode(J1Y, INPUT);
  pinMode(J1X, INPUT);
  pinMode(J2X, INPUT);

  initRadio();
}

void loop() {
  readJoiticks(j1x, j1y, j2x);
  Serial.print(j1x);
  Serial.print(" : ");
  Serial.print(j1y);
  Serial.print(" - ");
  Serial.println(j2x);

  //const char text[] = "Hello, World!";
  //bool result = radio.write(&text, sizeof(text));

  char payload[32];
  snprintf(payload, sizeof(payload), "%d,%d,%d", j1x, j1y, j2x);

  bool result = radio.write(payload, strlen(payload) + 1);  // +1 pentru '\0'

  if (result) {
    Serial.print("Send: ");
    Serial.println(payload);
  } else {
    Serial.println("Transmission failed");
  }

  delay(500);
}
