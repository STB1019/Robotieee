#include <SoftwareSerial.h>
String message = "";
int switchState = LOW;
const int SWITCH = 7;
const int LED_RED = 6;
const int LED_GREEN = 5;
const int RX = 4;
const int TX = 3;
SoftwareSerial bluetooth(RX, TX);
void setup() {
  pinMode(SWITCH, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  Serial.begin(9600); //set baud rate
  bluetooth.begin(9600); //set baud rate
}

void loop() {
  switchState = digitalRead(SWITCH);
  while ((bluetooth.available()) && (switchState == LOW)) {
    message += char(bluetooth.read());
    switchState = digitalRead(SWITCH);
    if (Serial.available()) {
      bluetooth.write(Serial.read());
    }
  }

  if (!bluetooth.available()) {
    if (message != "") {
      digitalWrite(LED_GREEN, HIGH);
      Serial.println(message);
      delay(500);
      digitalWrite(LED_GREEN, LOW);
    }
    message = "";
  }
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (switchState == HIGH) {
    digitalWrite(LED_RED, HIGH);
    bluetooth.write("ROGER");
    delay(500);
    digitalWrite(LED_RED, LOW);
  }
}

