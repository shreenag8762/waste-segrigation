// Define pins for ultrasonic sensors
// Sensor 1
const int TRIG_PIN1 = 2;
const int ECHO_PIN1 = 3;
// Sensor 2
const int TRIG_PIN2 = 4;
const int ECHO_PIN2 = 5;
// Sensor 3
const int TRIG_PIN3 = 6;
const int ECHO_PIN3 = 7;

// Constants for bin depth and calculations
const int BIN_DEPTH = 30; // Depth of bins in centimeters
const int MEASUREMENTS = 5; // Number of measurements to average
const int MEASURE_INTERVAL = 1000; // Interval between readings in milliseconds

// Function declarations
int measureDistance(int trigPin, int echoPin);
int calculateFillLevel(int distance);
int getAveragedMeasurement(int trigPin, int echoPin);

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  
  // Configure ultrasonic sensor pins
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(TRIG_PIN3, OUTPUT);
  pinMode(ECHO_PIN3, INPUT);
  
  // Ensure trigger pins start LOW
  digitalWrite(TRIG_PIN1, LOW);
  digitalWrite(TRIG_PIN2, LOW);
  digitalWrite(TRIG_PIN3, LOW);
  
  // Allow sensors to settle
  delay(1000);
}

void loop() {
  // Get averaged measurements for each bin
  int level1 = getAveragedMeasurement(TRIG_PIN1, ECHO_PIN1);
  int level2 = getAveragedMeasurement(TRIG_PIN2, ECHO_PIN2);
  int level3 = getAveragedMeasurement(TRIG_PIN3, ECHO_PIN3);
  
  // Send data in the format expected by the web interface
  Serial.print("Bin1:");
  Serial.print(level1);
  Serial.print(",Bin2:");
  Serial.print(level2);
  Serial.print(",Bin3:");
  Serial.print(level3);
  Serial.println(); // Add newline
  
  // Wait before next reading
  delay(MEASURE_INTERVAL);
}

// Function to get averaged measurements
int getAveragedMeasurement(int trigPin, int echoPin) {
  long sum = 0;
  
  // Take multiple measurements
  for(int i = 0; i < MEASUREMENTS; i++) {
    int distance = measureDistance(trigPin, echoPin);
    int fillLevel = calculateFillLevel(distance);
    sum += fillLevel;
    delay(50); // Short delay between measurements
  }
  
  // Return averaged result
  return sum / MEASUREMENTS;
}

// Function to measure distance using ultrasonic sensor
int measureDistance(int trigPin, int echoPin) {
  // Clear trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Send 10μs pulse to trigger
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure the response
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in centimeters
  // Speed of sound = 343m/s = 34300cm/s
  // Distance = (Time × Speed) ÷ 2
  // Distance = (duration × 0.0343) ÷ 2
  int distance = duration * 0.0343 / 2;
  
  return distance;
}

// Function to convert distance to fill level percentage
int calculateFillLevel(int distance) {
  // Ensure distance is within valid range
  if(distance > BIN_DEPTH) distance = BIN_DEPTH;
  if(distance < 0) distance = 0;
  
  // Convert to percentage
  // Empty bin (maximum distance) = 0%
  // Full bin (minimum distance) = 100%
  int fillLevel = 100 - ((distance * 100) / BIN_DEPTH);
  
  // Constrain result to 0-100 range
  if(fillLevel < 0) fillLevel = 0;
  if(fillLevel > 100) fillLevel = 100;
  
  return fillLevel;
}
