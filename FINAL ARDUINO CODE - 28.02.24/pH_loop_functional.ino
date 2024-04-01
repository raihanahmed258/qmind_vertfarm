#include <Arduino.h>
void AddpHSolution(float current_volume, float ph_difference, float currentpH);

// Pin Definitions
int relay1 = 5;
int relay2 = 6;
int pumpPin = 4; // Both nutrient and pH correction pumps connected to pin 4
#define TRIG_PIN 2
#define ECHO_PIN 3
const int analogPin = A0;  // pH sensor connected to analog pin A0

// Ultrasonic Sensor Variables
float bucketRadius = 15.24; // cm
float HCSR04MountingHeight = 38; // cm
float bucketHeight = 38; // cm

// Nutrient Addition Variables
const float volumePerUnit = 10000; // mL
const float nutrientToAddPerUnit = 10.5; // mL
const float flowRate = 1.12; // mL/s

// pH Correction Variables
const float a = 1.53e+01; // Constant term
const float b = 4.25e+02; // cumulative_ph_drop coefficient
const float c = -4.70e+01; // (cumulative_ph_drop)^2 coefficient
float reference_volume = 15300; // Reference volume for pH correction
float pH_flowRate = 1.12; // Flow rate for pH correction solution, in mL/s
float pH_tolerance = 0.2; // pH tolerance for correction

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 bits per second
  Serial.println("Setup Complete"); // Test Serial output

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(relay1, LOW); // Ensure relays are off
  digitalWrite(relay2, LOW); // Ensure relays are off

  // Perform an initial pH reading and print it
  int initialAnalogReading = analogRead(analogPin);
  float initialpH = convertToPH(initialAnalogReading);
  Serial.print("Initial pH: ");
  Serial.println(initialpH);
}

void loop() {
  static unsigned long lastAdjustmentTime = 0;
  const unsigned long adjustmentInterval = 10000; // 10 seconds
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastAdjustmentTime > adjustmentInterval) {
    // Calculate current bucket volume and pH
    float bucketVolume = calculateBucketVolume();
    int analogReading = analogRead(analogPin);
    float currentpH = convertToPH(analogReading);
    float desiredpH = 6.5; // Example desired pH value

    if (abs(currentpH - desiredpH) > pH_tolerance) {
      // Since AddpHSolution prints the current pH, we don't need to print it again here
      AddpHSolution(bucketVolume, currentpH - desiredpH, currentpH);
      lastAdjustmentTime = currentMillis; // Update the last adjustment time
    } else {
      // pH is within tolerance, no adjustment needed
      // Optionally, print this only if you need to know no adjustment was made
      Serial.println("pH is within tolerance. No adjustment needed.");
    }
  }
}

//-------------------------PUMP RELAY CONTROL ----------------------------//
void turnRelayOn(int pin) {
  digitalWrite(pin, HIGH); // Set the pin to HIGH, turning the relay on.
}

void turnRelayOff(int pin) {
  digitalWrite(pin, LOW); // Set the pin to LOW, turning the relay off.
}

//-------------------------MEASUREMENTS----------------------------//
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = (duration / 2.0) / 29.1;
  return distance;
}

float calculateVolume(float height) {
  return (3.14159265 * sq(bucketRadius) * height) / 1000; // Convert to liters
}

float calculateBucketVolume() {
  float distance = measureDistance();
  float bucketFillHeight = HCSR04MountingHeight - distance;
  float bucketVolume = calculateVolume(bucketFillHeight);
  return bucketVolume * 1000; // Convert liters to milliliters
}

float convertToPH(int analog) {
  return analog * 0.0333 - 2.8;
}

void AddpHSolution(float current_volume, float ph_difference, float currentpH) {
  float polynomialValue = a + (b * ph_difference) + (c * pow(ph_difference, 2));
  float predictedPHSol = polynomialValue * (current_volume / reference_volume);

  float PHoperationTimeInSeconds = predictedPHSol / pH_flowRate;
  unsigned long PHoperationTimeInMilliseconds = PHoperationTimeInSeconds * 1000;

  Serial.print("Current pH: ");
  Serial.println(currentpH); // Make sure you have a way to access currentpH here, either by passing it as a parameter or making it global.

  turnPumpOnForDuration(pumpPin, PHoperationTimeInMilliseconds);

  // Serial output for debugging
  Serial.print("pH Solution Added: ");
  Serial.print(predictedPHSol);
  Serial.println(" mL");
  Serial.print("pH Pump Operation Time: ");
  Serial.print(PHoperationTimeInMilliseconds);
  Serial.println(" milliseconds");
}

void turnPumpOnForDuration(int pin, unsigned long duration) {
  digitalWrite(pin, LOW); // Assuming active low for the pump to be ON
  delay(duration); // Wait for the duration in milliseconds
  digitalWrite(pin, HIGH); // Turn the pump off
  delay(10000);
}
