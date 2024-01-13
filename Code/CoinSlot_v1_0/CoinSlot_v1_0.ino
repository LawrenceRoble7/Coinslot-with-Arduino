#include "TM1637Display.h"

// Initialize All Pins
const byte inputImpulse = 2;
const byte Relay = 3;
const byte buzzer = 4;
const byte ClockPin = 5;
const byte DataPin = 6;
const byte addBtn = 7;
const byte minusBtn = 8;
const byte grnLED = 9;
const byte potentiometer = A0;

bool minusBtn_state;
bool addBtn_state;
int minPerPeso = 1;

TM1637Display display(ClockPin, DataPin);

unsigned long countPulse = 0;
unsigned long addMins;
long mins = 0;
unsigned long secs = 0;

volatile int impulseState;
void coinSlot() {
  impulseState = digitalRead(inputImpulse);

  if (!impulseState) {
    countPulse++;
  }
  delay(70);
}

void setup() {
  display.setBrightness(0x0c);
  Serial.begin(9600);

  pinMode(inputImpulse, INPUT_PULLUP);
  pinMode(Relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(potentiometer, INPUT);
  pinMode(addBtn, INPUT_PULLUP);
  pinMode(minusBtn, INPUT_PULLUP);
  pinMode(grnLED, OUTPUT);

  digitalWrite(Relay, 1);
  noTone(buzzer);
  Serial.println("Please wait for a moment...");
  delay(2000);
  Serial.println("The system is now ready!");
  delay(1000);
  Serial.println();

  attachInterrupt(digitalPinToInterrupt(inputImpulse), coinSlot, CHANGE);
}

void loop() {
  UpdateTime();

  addBtn_state = digitalRead(addBtn);
  minusBtn_state = digitalRead(minusBtn);

  if (addBtn_state == 0 && minusBtn_state == 0) {
    digitalWrite(grnLED, 1);
    ResetAll();
    delay(1000);
    SetTimePesoValue();
  }

  if (secs <= 0 && mins != 0) {
    mins--;
    secs = 59;
  }

  if (mins >= 0 && secs > 0) {
    if (mins > 99) {
      display.showNumberDecEx(mins, 0, false, 4, 0);
    } else {
      display.showNumberDecEx(mins, 0b01000000, true, 2, 0);
      display.showNumberDecEx(secs, 0, true, 2, 2);
    }
    digitalWrite(Relay, 0);
  } else {
    ResetAll();
  }

  if (mins >= 0 && secs > 0) {
    secs--;
  }

  Serial.print("Time: ");
  Serial.print(mins);
  Serial.println(" mins");
  Serial.println();
  Serial.print("countPulse: ");
  Serial.println(countPulse);

  NoTimeAlarm();
  delay(TimeDelay());
}

void NoTimeAlarm() {
  if ((mins == 0 && secs == 59) || (mins == 0 && secs == 57) || (mins == 0 && secs == 55)) {
    tone(buzzer, 3000);
  } else if (secs == 58 || secs == 56 || secs == 54) {
    noTone(buzzer);
  } else if ((mins == 0 && secs == 6) || (mins == 0 && secs == 4) || (mins == 0 && secs == 2)) {
    tone(buzzer, 3000);
  } else if (secs == 5 || secs == 3 || secs == 1) {
    noTone(buzzer);
  }
}

void UpdateTime() {
  if (countPulse > 0) {
    addMins = countPulse * minPerPeso;
    mins += addMins;
    countPulse = 0;
  }
}

int TimeDelay() {
  int potentiometeVal = analogRead(potentiometer);
  if (potentiometeVal <= 100) {
    return 100;
  } else if (potentiometeVal >= 1000) {
    return 1000;
  } else {
    return potentiometeVal;
  }
}

void SetTimePesoValue() {
  while ((addBtn_state != 0 && minusBtn_state != 0)) {
    if (addBtn_state == 0) {
      minPerPeso++;
    } else if (minusBtn_state == 0) {
      minPerPeso--;
      if (minPerPeso <= 1) {
        minPerPeso = 1;
      }
    }

    delay(500);
    display.showNumberDecEx(minPerPeso, 0, false, 4, 0);

    addBtn_state = digitalRead(addBtn);
    minusBtn_state = digitalRead(minusBtn);
  }
  digitalWrite(grnLED, 0);
}

void ResetAll() {
  mins = 0;
  secs = 0;
  digitalWrite(Relay, 1);
  display.showNumberDecEx(mins, 0b01000000, true, 2, 0);
  display.showNumberDecEx(secs, 0, true, 2, 2);
}