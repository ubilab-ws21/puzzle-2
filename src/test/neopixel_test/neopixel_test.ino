/*
  Neo_Pixel Test Code
    using ESP-32
*/

#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 22  

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

#define SSID "ubilab_wifi"   // Wifi SSID
#define PWD "ohg4xah3oufohreiPe7e"   // Wifi Password

#define NAME "Puzzle_2"  // Host Name

void setup() {
  
   Serial.begin(9600);
   
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

  /*********************
    NeoPixel Initialize
   *********************/
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  for(int i=0;i<NUMPIXELS;i++)   
  {
    pixels.setBrightness(10);
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
  delay(500);
}

void loop() {

   for(int i=0;i<NUMPIXELS;i++)   
   {
      pixels.setBrightness(10);
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      pixels.show();
   }
}
