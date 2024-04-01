//-------------------------MOTOR----------------------------//
int relay1 = 5; // Relay 1 connected to pin 5
int relay2 = 6; // Relay 2 connected to pin 6

//-------------------------ULTRASONIC SENSOR----------------------------//
#define TRIG_PIN 2  // Define the trigger pin for the ultrasonic sensor
#define ECHO_PIN 3  // Define the echo pin for the ultrasonic sensor
float bucketRadius = 15.24; // Radius of the bucket in cm (30.48 cm diameter / 2)
float HCSR04MountingHeight = 38; // Height at which the sensor is mounted above the bucket, in cm
float bucketHeight = 38; // Total height of the bucket in cm

//-------------------------pH----------------------------//
const int analogPin = A0;  // Define the analog pin
float pH;



void setup() {
  //-------------------------SERIAL PORT----------------------------//
  pinMode(7, OUTPUT); // Initialize digital pin 7 as an output.
  Serial.begin(9600); // Start serial communication at 9600 baud rate.

  //-------------------------MOTOR----------------------------//
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  // Initialize relays to OFF (Assuming LOW means OFF)
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);

  //-------------------------ULTRASONIC SENSOR----------------------------//
  Serial.begin(9600); // Start serial communication at 9600 baud
  pinMode(TRIG_PIN, OUTPUT); // Set the TRIG_PIN as an output
  pinMode(ECHO_PIN, INPUT);  // Set the ECHO_PIN as an input  

  //-------------------------pH----------------------------//
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  //-------------------------SERIAL PORT----------------------------//
  if (Serial.available() > 0) { // Check if data is available to read.
    char command = Serial.read(); // Read the incoming byte.
    
    if (command == '1') {
      turnRelayOn(7);
    } else if (command == '0') {
      turnRelayOff(7);
    }
  }

  //-------------------------pH----------------------------//
  int analogValue = analogRead(analogPin);  // Read the analog value from A0
  Serial.print("pH Value: ");
  Serial.println(convertToPH(analogValue));
  delay(1000);  // Delay for one second before the next reading

  //-------------------------MOTOR----------------------------//
  // Turn on Relay 1 (Motor 1)
  if (analogValue > 6.5) {
    digitalWrite(relay1, HIGH);
    delay(100); // Wait for 1 second

    // Turn off Relay 1 (Motor 1)
    digitalWrite(relay1, LOW);
    delay(100); // Wait for 1 second

    // Turn on Relay 2 (Motor 2)
    digitalWrite(relay2, HIGH);
    delay(100); // Wait for 1 second

    // Turn off Relay 2 (Motor 2)
    digitalWrite(relay2, LOW);
    delay(10000); // Wait for 1 second
  }
  

  //-------------------------ULTRASONIC SENSOR----------------------------//
  float distance = measureDistance(); // Measure the distance to the water surface
  float waterFillPercentage; // Variable to hold the water fill percentage

  // Calculate the fill height of the water in the bucket
  float bucketFillHeight = HCSR04MountingHeight - distance;

  // Check if the bucket fill height is logically greater than the bucket's physical height
  if (bucketFillHeight > bucketHeight || bucketFillHeight < 0) {
    // If so, set all measurements to 0 to indicate an error or incorrect setup
    Serial.println("Error: Measured fill height is outside of logical range.");
    distance = 0;
    bucketFillHeight = 0;
    waterFillPercentage = 0;
  } else {
    // Calculate the volume and fill percentage if the measurement is within logical range
    float totalVolume = calculateVolume(bucketHeight);
    float bucketVolume = calculateVolume(bucketFillHeight);
    waterFillPercentage = (bucketVolume / totalVolume) * 100;
  }

  // Print the results to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Bucket Fill Height: ");
  Serial.print(bucketFillHeight);
  Serial.println(" cm");
  Serial.print("Water Fill Percentage: ");
  Serial.print(waterFillPercentage);
  Serial.println(" %");
  Serial.print(analogValue);

  delay(1000); // Delay a second before taking the next reading

}

//-------------------------SERIAL PORT FUNCTIONS----------------------------//
void turnRelayOn(int pin) {
  digitalWrite(pin, HIGH); // Set the pin to HIGH, turning the relay on.
}

void turnRelayOff(int pin) {
  digitalWrite(pin, LOW); // Set the pin to LOW, turning the relay off.
}

//-------------------------ULTRASONIC SENSOR FUNCTIONS----------------------------//
float measureDistance() {
  // Send a 10us pulse to TRIG_PIN to trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the time for the echo to return
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance based on the speed of sound (340 m/s)
  float distance = (duration / 2.0) / 29.1;
  return distance;
}

float calculateVolume(float height) {
  // Calculate the volume of water in the bucket based on the height of the water column
  // Assuming the bucket is a perfect cylinder, volume = π * r^2 * height
  // Volume is returned in liters by converting cm^3 to liters (divide by 1000)
  return (3.14159265 * sq(bucketRadius) * height) / 1000;
}

//-------------------------pH FUNCTION----------------------------//
// pH: 11.5 - Reading: 475
// pH 7 - Reading 415

// m = (11.5 - 7) / (475 - 415) = 0.075
// b = 7 - m * 415 = -24.125

// This uses the calculations y=mx+b model from above
float convertToPH(int analog) {
  return analog * 0.0333 - 2.8;
}
