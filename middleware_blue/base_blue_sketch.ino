#include <SoftwareSerial.h>
String message = "";
char *raw;
int c;
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
    raw += bluetooth.read();
    switchState = digitalRead(SWITCH);
  }

  if (!bluetooth.available()) {
    if (raw != "") {
      blinkLED(LED_GREEN, HIGH, 500, LOW);
      move(raw);
      raw = "";
      blinkLED(LED_GREEN, HIGH, 500, LOW);
    }
  }
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (switchState == HIGH) {
    blinkLED(LED_RED, HIGH, 500, LOW);
    Serial.write("ROGER");
    bluetooth.write("ROGER");
    blinkLED(LED_RED, HIGH, 500, LOW);
  }
}

void blinkLED(int led, int one, int sec, int two) {
  digitalWrite(led, one);
  delay(sec);
  digitalWrite(led, two);
}

void move(char raw) {
  Serial.println(raw);
  Serial.println(raw, DEC);
  Serial.println(raw, HEX);
}
