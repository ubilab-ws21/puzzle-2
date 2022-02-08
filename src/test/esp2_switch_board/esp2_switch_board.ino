/* 
   Switch Board Code for Puzzle-2
*/

#include <WiFi.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


/*********************
     NeoPixel
*********************/
#define PIN 23          // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 8     // Number of Pixels in NeoPixel
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500    // Time (in milliseconds) to pause between pixels


/*********************
     Switch Panel
*********************/
// Pins of toggle switches
int button[] = {13,27,26,33,35,15,4,18};


/*********************
     OLED Display
*********************/
#define SCREEN_WIDTH 128         // OLED display width, in pixels
#define SCREEN_HEIGHT 64         // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);   // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)


/*********************
  Other Declarations
*********************/
//Use Internet connection
#define useNetwork

//Timer Declaration
unsigned long int progTimer;
int fullTime=60000;    //Time after which ESP32 restarts

//Puzzle Declarations
int flag=0;
int randNo, bitCode;
char *newWord; 
char *words[] = {"eeltcric","nteowrks","magnteci","paritcel","dissaetr","collsape"};
int bitCodes[6][NUMPIXELS] = {{0,1,1,1,1,0,0,0},
                              {0,1,1,1,1,0,0,0},
                              {0,0,0,0,1,1,1,1},
                              {0,0,0,1,1,0,1,1},
                              {0,0,0,1,1,1,1,0},
                              {0,0,0,0,1,1,1,0}};
int code[NUMPIXELS] = {0,0,0,0,0,0,0,0};
int currCode[NUMPIXELS] = {0,0,0,0,0,0,0,0};

boolean CodeCmp(int a[NUMPIXELS], int b[NUMPIXELS])
{
  for(int i=0;i<NUMPIXELS;i++) if(a[i]!=b[i]) return false;
  return true;
}

// Speed of Serial Communication
#define SERIAL_COMSPEED 9600

// Credentials for network SSID and PWD
#define SSID "ubilab_wifi"
#define PWD "ohg4xah3oufohreiPe7e"

// Device can be found on the network using this name
#define NAME "Puzzle2-ESP1"  //Name of the ESP: 0/1
#define OTA_PWD "sss21"      // PWD for OTA

// MQTT Server
#define mqtt_server "earth.informatik.uni-freiburg.de"
WiFiClient puzzle_2;
PubSubClient client(puzzle_2);
#define MAX_MSG 50
char msg[MAX_MSG] = {'\0'};

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("puzzle-2")) {
      Serial.println("connected");
      client.subscribe("2/ledstrip/serverroom");
      client.setCallback(callback);
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}


void setup() {
  // Initialize the Serial Port
  Serial.begin(SERIAL_COMSPEED);
  
  #ifdef useNetwork
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
          MQTT Server
    *********************/
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    client.subscribe("2/ledstrip/serverroom");
    
    /*********************
       multicast DNS
    *********************/
    // Set name of this device
    if (!MDNS.begin(NAME)) {
      Serial.println("Setting up MDNS responder!");
    }
    // The service this device hosts (example)
    MDNS.addService("_escape", "_tcp", 2000);
  
    // OTA setup in separate function
    setupOTA();
 #endif


  /*********************
    Puzzle Initialize
  *********************/
  randNo =  random(0,5);
  newWord = words[randNo];
  for(int k=0; k<NUMPIXELS; k++)
  {
    code[k] = bitCodes[randNo][k];
  }
  
 
  /*********************
       OLED Display
  *********************/
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {       // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Guess the word?\n");
  display.println("Note the wrong positions\n");
  delay(100);
  display.setTextSize(2);
  display.println(newWord);  
  display.display(); 
  delay(100); 

  
  /*********************
   NeoPixel Initialize
  *********************/
  pixels.begin();   
  pixels.clear();
  delay(500);


  /**************************
   Switches Mode Initialize
  **************************/
  for(int j=0; j<NUMPIXELS; j++)
  {
    pinMode(button[j],INPUT);
  }
}

void loop() {
  #ifdef useNetwork
    // Needs to be called to check for external update request
    ArduinoOTA.handle();
    //MQTT Loop
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    if(flag==1) client.publish("2/ledstrip/serverroom","status","solved");  
  #endif

  //Checks whether the switch sequence matches or not
  if(CodeCmp(currCode,code) == true)
  {
    flag=1;
    pixels.fill(pixels.Color(0, 0, 255));
    pixels.show();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,10);
    display.println("Puzzle\n");
    display.println("Solved\n");
    display.display();
  }

  //Reads the current sequence of switches
  for(int j=0;j<NUMPIXELS; j++)
  {
    currCode[j] = digitalRead(button[j]);
  }

  for(int i=0;i<NUMPIXELS;i++)   
  {
     pixels.setBrightness(10);
     if(currCode[i] == 1 && flag==0)
     {
       pixels.setPixelColor(((NUMPIXELS-1)-i), pixels.Color(0, 255, 0));
       pixels.show();
     }
     else
     {
       pixels.setPixelColor(((NUMPIXELS-1)-i), pixels.Color(255, 0, 0));
       pixels.show();
     }
  }
  
 /*****************************
   Puzzle Reset after 15 mins
 *****************************/ 
  progTimer = millis();
  if(progTimer >= fullTime){
    esp_restart();
  }  
}

// MQTT Callback function
void callback(char* topic, byte* message, unsigned int length) {
  // Convert to cstring
  int len = min((int)length, (int)(MAX_MSG-1));
  memcpy(&msg[0], message, len);
  msg[len] = '\0'; 

  Serial.printf("MQTT msg on topic: %s: %s\n", topic, &msg);
}

/**************************
     Setup OTA Updates
**************************/
void setupOTA() {
  // Set name of this device (again)
  ArduinoOTA.setHostname(NAME);
  // Set a password to protect against uploads from other parties
  ArduinoOTA.setPassword(OTA_PWD);
  // CB function for when update starts
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating");
  });
  // CB function for when update starts
  ArduinoOTA.onEnd([]() {
    Serial.println("Finished updating");
  });
  // CB function for progress
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Update at %u % \n", progress / (total / 100));
  });
  // CB function for when update was interrupted
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.println("Error updating");
    ESP.restart();
  });
  // Start OTA update service
  ArduinoOTA.begin();
}
