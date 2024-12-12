// library Import
#include <Wire.h>              // Built-in library for I2C communication
#include <LiquidCrystal_I2C.h> // Library Name: LiquidCrystal_I2C by Frank de Brabander

// Variabel Initialization for Thermistor
int ThermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// Variabel Initialization for Button and SSR
#define BUTTON_PIN 12
#define SSR_PIN 13
bool controlActive = false;
bool buttonPressed = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  // Serial Communication Initialization
  Serial.begin(9600);

  // PinMode Initialization
  pinMode(SSR_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // LCD Initialization
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  lcd.print("Sistem Dimulai!");
  delay(2000);
  lcd.clear();
}

void loop()
{
  // Thermistor Calculation
  // Reference: https://www.anakkendali.com/2020/01/17/belajar-arduino-mengakses-sensor-suhu-ntc-thermistor-10k/
  Vo = analogRead(ThermistorPin);       // Reads the analog value from the thermistor.
  R2 = R1 * (1023.0 / (float)Vo - 1.0); // Calculates the resistance of the thermistor.
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15; // Applies the Steinhart-Hart equation to find the temperature in Celsius (Tc).

  // Serial and LCD Print
  Serial.print("Temperature: ");
  Serial.print(Tc);
  Serial.println(" C");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(Tc);
  lcd.print(" C");

  // Button Control
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    if (!buttonPressed)
    {
      // Checks if the button is pressed to toggle controlActive.
      controlActive = !controlActive;
      buttonPressed = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(controlActive ? "Kontrol ON" : "Kontrol OFF");
      delay(1000); // Debounces the button press to prevent multiple toggles.
    }
  }
  else
  {
    buttonPressed = false;
  }

  if (controlActive)
  {
    /* If controlActive is true:
       Turns the heater OFF if Tc >= 60°C.
       Turns the heater ON if Tc <= 50°C.
       Updates the LCD and serial monitor with the heater status.
    */
    if (Tc >= 60)
    {
      digitalWrite(SSR_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Heater OFF ");
      Serial.println("SSR OFF (Temp >= 60C)");
    }
    else if (Tc <= 50)
    {
      digitalWrite(SSR_PIN, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Heater ON  ");
      Serial.println("SSR ON (Temp <= 50C)");
    }
  }
  else
  {
    digitalWrite(SSR_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Heater OFF ");
  }

  delay(500); // Waits for 500 milliseconds before the next loop iteration.
}
