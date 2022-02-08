/* 
              ESP-32 Wifi Test

  [Blinks internal LED upon successful connection]
  
*/


#include <WiFi.h>

// Pin of internal LED of the ESP32
#define LED_PIN 2

// Speed of Serial Communication
#define SERIAL_COMSPEED 9600

// Credentials for network SSID and PWD
#define SSID "ubilab_wifi"
#define PWD "ohg4xah3oufohreiPe7e"

// Device can be found on the network using this name
#define NAME "Puzzle_2"

// Setup function that runs once upon startup or reboot
void setup() {
  // Initialize the Serial Port
  Serial.begin(SERIAL_COMSPEED);

  pinMode(LED_PIN,OUTPUT);

  /*********************
     WiFi Connection
   *********************/
  Serial.print("Connecting to ");
  Serial.println(SSID);
  // Set name passed to AP while connection
  WiFi.setHostname(NAME);
  // Connect to AP
  WiFi.begin(SSID, PWD);
  // Wait while not connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  // Print IP
  Serial.println("\nWiFi connected.\nIP Adress: ");
  Serial.println(WiFi.localIP());
}
 
void loop(){
  if(WiFi.status() == WL_CONNECTED){
  delay(500);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
  }
  else{
  digitalWrite(LED_PIN, LOW);
  }
}
