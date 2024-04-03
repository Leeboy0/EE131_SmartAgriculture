#include <LiquidCrystal.h>
#include <DHT.h>

// Define sensor and LCD pins
#define DHTPIN A0          // DHT11 sensor pin
#define DHTTYPE DHT11      // DHT11 sensor type
#define RS 7               // LCD pin
#define EN 6               // LCD pin
#define D4 5               // LCD pin
#define D5 4               // LCD pin
#define D6 3               // LCD pin
#define D7 2               // LCD pin
#define JS_BTN 12          // Joystick button pin

// LED pins
#define LED_HEATER 10
#define LED_COOLER 11
#define LED_DECREASE_HUMIDITY 9
#define LED_INCREASE_HUMIDITY 8

// Joystick pins
#define JOY_X A1           // Joystick X-axis pin

// Initialize DHT sensor and LCD display
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Settings
int setTemp = 22;          // Desired temperature setting
int setHumidity = 55;      // Desired humidity setting
bool displayMode = false;  // false for temperature, true for humidity

// Variables for API temperature
float apiTemperature = 0.0; // Placeholder for temperature received from the API

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  pinMode(JS_BTN, INPUT_PULLUP);

  // Initialize LED pins as outputs
  pinMode(LED_HEATER, OUTPUT);
  pinMode(LED_COOLER, OUTPUT);
  pinMode(LED_DECREASE_HUMIDITY, OUTPUT);
  pinMode(LED_INCREASE_HUMIDITY, OUTPUT);
  
  lcd.print("Setup Complete");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read humidity from DHT sensor
  float currentHumidity = dht.readHumidity();

  // Check for serial data from API
  if (Serial.available() > 0) {
    apiTemperature = Serial.parseFloat();
    // Clear the serial buffer
    while(Serial.available() > 0) Serial.read();
  }

  toggleDisplayMode();
  adjustSetValues();

  // Display the current settings and readings
  displayClimate();

  // Adjust LEDs based on current climate conditions
  adjustClimateLEDs(apiTemperature, currentHumidity);

  delay(100); // Loop delay to manage display refresh and responsiveness
}

void toggleDisplayMode() {
  static unsigned long lastPressTime = 0;
  unsigned long currentTime = millis();
  if (digitalRead(JS_BTN) == LOW && currentTime - lastPressTime > 200) { // Debounce delay
    displayMode = !displayMode;
    lastPressTime = currentTime;
    delay(100); // Debounce delay
  }
}

void adjustSetValues() {
  int joyX = analogRead(JOY_X);
  
  if (!displayMode) { // Adjusting temperature
    if (joyX > 800) {
      setTemp++;
    } else if (joyX < 200) {
      setTemp--;
    }
  } else { // Adjusting humidity
    if (joyX > 800) {
      setHumidity++;
    } else if (joyX < 200) {
      setHumidity--;
    }
  }
}

void displayClimate() {
  lcd.clear();
  if (!displayMode) {
    lcd.setCursor(0, 0);
    lcd.print("API Temp: ");
    lcd.print(apiTemperature);
    lcd.setCursor(0, 1);
    lcd.print("Set Temp: ");
    lcd.print(setTemp);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print(dht.readHumidity());
    lcd.setCursor(0, 1);
    lcd.print("Set Humid: ");
    lcd.print(setHumidity);
  }
}

void adjustClimateLEDs(float currentTemp, float currentHumidity) {
  digitalWrite(LED_HEATER, currentTemp < setTemp ? HIGH : LOW);
  digitalWrite(LED_COOLER, currentTemp > setTemp ? HIGH : LOW);
  digitalWrite(LED_INCREASE_HUMIDITY, currentHumidity < setHumidity ? HIGH : LOW);
  digitalWrite(LED_DECREASE_HUMIDITY, currentHumidity > setHumidity ? HIGH : LOW);
}