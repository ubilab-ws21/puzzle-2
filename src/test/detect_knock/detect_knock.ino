/*
  Knock Sensor
  
  Tested using Arduino UNO
*/


// these constants won't change:
const int ledPin = 13;      // LED connected to digital pin 13
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not
int count = 0;

// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = LOW;         // variable used to store the last LED status, to toggle the light
int code =4;
void setup() {
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  Serial.begin(9600);       // use the serial port
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);
  // if the sensor reading is greater than the threshold:
  if ((sensorReading >= threshold) &&(count < code)  ){
    Serial.println(sensorReading);
    // toggle the status of the ledPin:
    ledState = !ledState;
    // update the LED pin itself:
    digitalWrite(ledPin, ledState);
    // send the string "Knock!" back to the computer, followed by newline
    Serial.println("Knock!");
    count++;
   delay (500);
  }
  else if (count >= code)
  {
    Serial.println("Puzzle solved");
    count = 0;
    }
 }
  
