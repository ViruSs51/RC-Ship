#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Code of command for ship
// 00001111 - move to front
// 11110000 - move to back
// 10011110 - move to front and rotate to right(with motors)
// 01011111 - move to front and rotate to left(with motors)
// 01110001 - move to back and rotate to right(with motors)
// 11111010 - move to back and rotate to left(with motors)


#define CE_PIN 9
#define CSN_PIN 10
#define J1Y A2
#define J1X A3
#define J2Y A1
#define J2X A0

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "hghq{";

int j1x, j1y, center_j1x, center_j1y;
int j2x, j2y, center_j2x, center_j2y;

const char* command = "00001111";

void initRadio() {
  radio.begin();
  radio.setAddressWidth(5); 
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.setRetries(5, 15); 
  radio.stopListening();
}

void readJoistick(int &x, int &y, int PIN_X, int PIN_Y) {
  x = analogRead(PIN_X);
  y = analogRead(PIN_Y);
}

void joistickToMotor(int &x, int &y, int &center_x, int &center_y) {
  x = map(x, center_x, 1023, 0, 255);
  y = map(y, center_y, 1023, 0, 255);
}

void setup() {
  Serial.begin(9600);
  pinMode(J1Y, INPUT);
  pinMode(J1X, INPUT);
  pinMode(J2Y, INPUT);
  pinMode(J2X, INPUT);

  initRadio();

  // Centralize joistick
  readJoistick(center_j1x, center_j1y, J1X, J1Y);
  readJoistick(center_j2x, center_j2y, J2X, J2Y);
}

void loop() {  
  readJoistick(j1x, j1y, J1X, J1Y);
  readJoistick(j2x, j2y, J2X, J2Y);

  if (j2y >= center_j2y) {
    j2y = map(j2y, center_j2y, 1023, 0, 255);
    command = "00001111";
  } else {
    j2y = map(j2y, 0, center_j2y, 255, 0);
    command = "11110000";
  }

  if (j2x >= center_j2x) {
    j2x = map(j2x, center_j2x, 1023, 90, 120);
  } else {
    j2x = map(j2x, 0, center_j2x, 60, 90);
  }

  if (j1x >= center_j1x) {
    if (j1x > center_j1x) {
      if (command == "00001111") command = "10011110";
      else command = "01110001";
    }

    j1x = map(j1x, center_j1x, 1023, 0, 255);
  } else {
    if (j1x - 10 < center_j1x) {
      if (command == "00001111") command = "01011111";
      else command = "11111010";
    }

    j1x = map(j1x, 0, center_j1x, 255, 0);
  }

  char payload[50];
  snprintf(payload, sizeof(payload), "%s ,%d,%d,%d", command, j1x, j2x, j2y);

  bool result = radio.write(payload, sizeof(payload));

  if (result) {
    Serial.print("Sended: ");
  } else {
    Serial.print("Transmission failed: ");
  }

  Serial.print(j1x);
  Serial.print(" : ");
  Serial.print(j1y);
  Serial.print(" - ");
  Serial.print(j2x);
  Serial.print(" : ");
  Serial.println(j2y);
}
