#include <SoftwareSerial.h>
String message = "";
String raw;
char c;
int unsigned counter;
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
  counter=0;
}

void loop() {
  switchState = digitalRead(SWITCH);
  while ((bluetooth.available()) && (switchState == LOW)) {
    c = bluetooth.read();
    raw += c;
    switchState = digitalRead(SWITCH);
  }

  if (!bluetooth.available()) {
    if (raw != "") {
      mngLED(LED_GREEN, HIGH);
      move(raw);
      raw = "";
      mngLED(LED_GREEN, LOW);
    }
  }
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (switchState == HIGH) {
    mngLED(LED_RED, HIGH);
    Serial.write("ROGER[%u]\n",counter);
    bluetooth.write("ROGER[%u]\n",counter);
    mngLED(LED_RED,LOW);
    counter++;
  }
}

void mngLED(int led,int state){
  digitalWrite(led,state);
}
void blinkLED(int led, int one, int sec, int two) {
  digitalWrite(led, one);
  delay(sec);
  digitalWrite(led, two);
}

void move(String raw) {
  Serial.println("[RAW]");
  Serial.print(raw);
}
