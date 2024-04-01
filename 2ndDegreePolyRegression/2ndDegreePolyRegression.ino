// Polynomial coefficients specific to your equation
const float a = 1.53e+01; // Constant term
const float b = 4.25e+02; // cumulative_ph_drop coefficient
const float c = -4.70e+01; // (cumulative_ph_drop)^2 coefficient

// Reference volume
const float reference_volume = 15300.0; // AVERAGE ML IN THE BUCKET

float current_volume = 15000; // CURRENT VOLUME
float current_pH = 7.0; // CURRENT pH
float desired_pH = 6.8; // DESIRED pH
float cumulative_ph_drop = current_pH - desired_pH; // pH DROP

void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // Calculate the predicted nutrients to add
  float predictedNutrients = calculateNutrientsToAdd(cumulative_ph_drop, current_volume);

  // Print the result to the serial monitor
  Serial.print("Predicted Nutrients to Add: ");
  Serial.print(predictedNutrients);
  Serial.println(" mL");
}

void loop() {
  // Here you can put code that needs to be running repeatedly.
}

// Function to calculate the predicted nutrients to add based on the specific polynomial
float calculateNutrientsToAdd(float cumulative_ph_drop, float current_volume) {
  float polynomialValue = a + (b * cumulative_ph_drop) + (c * cumulative_ph_drop * cumulative_ph_drop);
  return polynomialValue * (current_volume / reference_volume);
}

//THEN DIVIDE predictedNutrients BY 1.12 TO GET NUMBER OF SECONDS TO RUN MOTOR FOR.