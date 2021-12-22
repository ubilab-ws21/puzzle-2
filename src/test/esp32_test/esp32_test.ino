#include <WiFi.h>

// Speed of Serial Communication
#define SERIAL_COMSPEED 9600

// Credentials for network SSID and PWD
#define SSID "TP-Link_703C"
#define PWD "Freiburg@2020"

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
}
