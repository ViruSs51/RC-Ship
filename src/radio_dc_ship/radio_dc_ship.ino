#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 5

const byte address[10] = "101010010";

RF24 radio(CE_PIN, CSN_PIN);

void initRadio() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void setup() {
  Serial.begin(9600);

  initRadio();
}

void loop() {
  if (radio.available()) {
    char payload[32] = "";
    int len = radio.getDynamicPayloadSize();
    radio.read(&payload, len);

    Serial.print("Received: ");
    Serial.println(payload);

    int j1x, j1y, j2x;
    sscanf(payload, "%d,%d,%d", &j1x, &j1y, &j2x);

    Serial.print("J1X: ");
    Serial.print(j1x);
    Serial.print(" J1Y: ");
    Serial.print(j1y);
    Serial.print(" J2X: ");
    Serial.println(j2x);
    Serial.println("---------------");
    delay(100);
  }
}
