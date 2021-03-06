
/*################################################################################# 

*          This is the main program for the ESP32 of our puzzle
* 
* There are two puzzle which has to be solved in order to open the server room:
* [1] Knock-Knock Puzzle  : This puzzle has a piezo element to detect the Knock
*                           pattern and unleasd the switch panel.
*                           
* {2] Switch Panel Puzzle : This puzzle has a series of toggle switches which 
*                           has to be turned on/off in a specific pattern which
*                           the player gets by solving the riddle displayed in
*                           the OLED Display.
* 
* When the puzzle is solved, the server room is opened.
* 
* MQTT Communication : There is a constant communication with the operator
*                      and the ARcape for the puzzle to work accordingly.
*                      
* The communication is done via various MQTT Topics:
* [1] 2/esp [start, stop] - control the state of the ESP
* [2] game/puzzle2 [inactive, active, solved] - displays hints using arcape
* 
* Authors:
* [1] Sougata Hanra
* [2] Santosh Kumar Prabhulingaiah
* [3] Shatakshi Sharma
   
#################################################################################*/

#include <WiFi.h>
#include <Wire.h>
#include "time.h"
#include <ESPmDNS.h>
#include <ESP32Servo.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>


/*********************
     Piezo Sensor
*********************/
int Puzzle = 0;
int checkZero = 0;
int piezoFlag = 0;
// these constants won't change:
const int knockSensorL = 32; // the left piezo is connected to analog pin 0
const int knockSensorR = 34; //the right piezo is connected to analog pin 1
const int threshold = 200;  // threshold value to decide when the detected sound is a knock or not
// these variables will change:
int count1 = 0;
int count2 = 0;
int sensorReadingL = 0;   // variable to store the value read from the left sensor pin
int sensorReadingR = 0;   // variable to store the value read from the right sensor pin


/*********************
        Servo
*********************/
Servo servo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 19;
int servoFlag = 0;

 
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
//Flag to activate the puzzle
bool puzzleStart;

//Use Internet connection
#define useNetwork

//Local Time Variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int CurrentHour, UnitHour, TenHour;
int hourNow=12;

//Create a JSON Document to read MQTT messages
StaticJsonDocument<300> mqtt_decoder;

//Puzzle Declarations
bool puzzleSolved=false;
int randNo, bitCode;
char *newWord; 
char *words[] = {"eeltcric","nteowrks","magnteci","paritcel","dissaetr","collsape"};
int bitCodes[6][NUMPIXELS] = {{1,0,0,0,0,1,1,1},
                              {1,0,0,0,0,1,1,1},
                              {1,1,1,1,0,0,0,0},
                              {1,1,1,0,0,1,0,0},
                              {1,1,1,0,0,0,0,1},
                              {1,1,1,1,0,0,0,1}};
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
#define NAME "Puzzle2-ESP0"  //Name of the ESP
#define OTA_PWD "sss21"      // PWD for OTA

// MQTT Server
#define mqtt_server "10.8.166.20"
WiFiClient puzzle_2;
PubSubClient client(puzzle_2);
bool retained = true;
#define MAX_MSG 50
char msg[MAX_MSG] = {'\0'};

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("puzzle-2")) {
      Serial.println("connected");
      client.subscribe("2/esp");
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
    client.subscribe("2/esp");
    
    /*********************
       multicast DNS
    *********************/
    // Set name of this device
    if (!MDNS.begin(NAME)) {
      Serial.println("Setting up MDNS responder!");
    }
    // The service this device hosts (example)
    MDNS.addService("_escape", "_tcp", 2000);
    setupOTA();

    /*********************
        Config Time
    *********************/ 
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getCurrentHour(); 
    hourNow = getCurrentHour();
 #endif
  
  /**************************
   Switches Mode Initialize
  **************************/
  for(int j=0; j<NUMPIXELS; j++)
  {
    pinMode(button[j],INPUT);
    currCode[j] = digitalRead(button[j]);
  }

  /*********************
          Servos
  *********************/
  servo.setPeriodHertz(50);  // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
  servo.write(160);  //set to default position 

  /*********************
  Config Time Variables
  *********************/ 
  // Init and get the time
  UnitHour = hourNow%10;
  TenHour =(hourNow/10)%10;
 
  
  /*********************
    Puzzle Initialize
  *********************/
  do
  {
    randNo = random(0,5);
    newWord = words[randNo];
    for(int k=0; k<NUMPIXELS; k++)
    {
      code[k] = bitCodes[randNo][k];
    }
  }while(CodeCmp(currCode,code) == true);

  
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
  display.println("Guess the word?");
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
    if(puzzleSolved==true){
      client.publish("game/puzzle2",createJson("status", "solved", ""));    
      client.publish("2/esp",createJson("status", "solved", ""));
    }
  #endif

  while(puzzleStart && !puzzleSolved){
    /*********************
       Piezo Sensor
    *********************/
    sensorReadingL = analogRead(knockSensorL);
    sensorReadingR = analogRead(knockSensorR);
     
    if(UnitHour==0) checkZero=1;
     
    if ((sensorReadingL >= threshold)&&(Puzzle == 0)){ 
      count1++;
      Serial.println("Knock1"); 
      makeBuzz();
      if (count1 >= TenHour) {
       Serial.println("knock 1 routine done");
       Puzzle = 1;
      }
    }
        
    else if ((sensorReadingR >= threshold)&&(Puzzle == 1)&&(UnitHour>0)){   
      count2++;  
      Serial.println("Knock2");
      makeBuzz();   
      if (count2 >= UnitHour) {
        Serial.println("knock 2 routine done");
        Puzzle = 2;
       }
     }
      
    else if (Puzzle == 2 ||(Puzzle == 1 && checkZero == 1)){  
    /*********************
             Servo
    *********************/
     for (pos = 160; pos >= 0; pos -= 1) { // goes from 0 degrees to 120 degrees in steps of 1 degree
      servo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(15);     // waits 15ms for the servo to reach the position
      Puzzle == 3;
     } 
     servo.detach();
     count1=0;
     count2=0;
    }
    
    //Checks whether the switch sequence matches or not
    if(CodeCmp(currCode,code) == true)
    {
      puzzleSolved = true;
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
       if(currCode[i] == 1 && !puzzleSolved)
       {
         pixels.setPixelColor(((NUMPIXELS-1)-i), pixels.Color(0, 255, 0));
         pixels.show();
       }
       else if (currCode[i] == 0 && !puzzleSolved)
       {
         pixels.setPixelColor(((NUMPIXELS-1)-i), pixels.Color(255, 0, 0));
         pixels.show();
       }
    }
  }
}

/*****************************
    Feedback for knock
*****************************/ 
void makeBuzz(){
  servo.write(130);
  delay(150);
  servo.write(160);
}

/*****************************
  Funtion to get Local Time
*****************************/ 
int getCurrentHour(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return 0;
  }
 CurrentHour = timeinfo.tm_hour;
 return CurrentHour;
}

/**************************
     MQTT Callback
**************************/
void callback(char* topic, byte* message, unsigned int length) {
  // Convert to cstring
  int len = min((int)length, (int)(MAX_MSG-1));
  memcpy(&msg[0], message, len);
  msg[len] = '\0'; 

  Serial.printf("MQTT msg on topic: %s: %s\n", topic, &msg);
  decodeMessage(topic, msg);
}

/**************************
     Setup OTA Updates
**************************/
void setupOTA() {
  ArduinoOTA.setHostname(NAME);           
  ArduinoOTA.setPassword(OTA_PWD);        
  ArduinoOTA.onStart([]() {               
    Serial.println("Start updating");
  });
  ArduinoOTA.onEnd([]() {                 
    Serial.println("Finished updating");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {    
    Serial.printf("Update at %u % \n", progress / (total / 100));
  });
  ArduinoOTA.onError([](ota_error_t error) {     
    Serial.println("Error updating");
    esp_restart();
  });
  ArduinoOTA.begin();     
}

/*************************************
   Function to create JSON message
*************************************/
char* createJson(char* method_s, char* state_s, char* data_s){
  StaticJsonDocument<300> doc;
  doc["method"] = method_s;
  doc["state"] = state_s;
  doc["data"] = data_s;
  static char JSON_String[300];
  serializeJson(doc, JSON_String);
  return JSON_String;
}

/*************************************
   Function to decode JSON message
   and work with Operator commands
*************************************/ 
void decodeMessage(char* topic, char* msg) {
  deserializeJson(mqtt_decoder, msg);
  const char* method_msg = mqtt_decoder["method"];
  const char* state_msg = mqtt_decoder["state"];
  const char* data_msg = mqtt_decoder["data"];
  
  if(strcmp(topic, "2/esp") == 0 && strcmp(method_msg, "trigger") == 0 && strcmp(state_msg, "on") == 0){
    puzzleStart=true;
    for(int k=0;k<5;k++){
      delay(600);
      makeBuzz();
    }
    client.publish("game/puzzle2",createJson("status", "active", ""),retained);
    client.publish("2/esp",createJson("status", "active", ""),retained);
  }
  if(strcmp(topic, "2/esp") == 0 && strcmp(method_msg, "trigger") == 0 && strcmp(state_msg, "off") == 0){
    puzzleStart=false;
    client.publish("game/puzzle2",createJson("status", "inactive", ""),retained);
    client.publish("2/esp",createJson("status", "inactive", ""),retained);
    delay(2000);
    esp_restart();
  }
}
