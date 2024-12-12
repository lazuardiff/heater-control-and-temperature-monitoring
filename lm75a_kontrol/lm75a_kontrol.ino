#include <Wire.h>

#define LM75A_ADDRESS 0x48
#define BUTTON_PIN 2
#define SSR_PIN 7

bool controlActive = false;
bool buttonPressed = false;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(SSR_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(SSR_PIN, LOW);
}

void loop() {
  float tempLM75A = readTemperatureLM75A();

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      controlActive = !controlActive;
      buttonPressed = true;
      Serial.print("CONTROL ");
      Serial.println(controlActive ? "STARTED" : "STOPPED");
    }
  } else {
    buttonPressed = false;
  }

  if (controlActive) {
    if (tempLM75A >= 90) {
      digitalWrite(SSR_PIN, LOW);
      Serial.println("SSR OFF (Temp >= 90C)");
    } else if (tempLM75A <= 60) {
      digitalWrite(SSR_PIN, HIGH);
      Serial.println("SSR ON (Temp <= 60C)");
    }
  } else {
    digitalWrite(SSR_PIN, LOW);
  }

  Serial.print("LM75A Temp: ");
  Serial.print(tempLM75A);
  Serial.println(" C");

  delay(500);
}

float readTemperatureLM75A() {
  Wire.beginTransmission(LM75A_ADDRESS);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) {
    return -273.15;
  }

  Wire.requestFrom(LM75A_ADDRESS, 2);
  if (Wire.available() == 2) {
    int msb = Wire.read();
    int lsb = Wire.read();
    int rawTemperature = (msb << 8) | lsb;
    rawTemperature >>= 7;
    if (rawTemperature > 255) {
      rawTemperature -= 512;
    }
    return rawTemperature * 0.5;
  }
  return -273.15;
}
