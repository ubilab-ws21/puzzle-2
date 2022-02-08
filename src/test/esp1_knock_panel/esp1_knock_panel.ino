/* 
   Knock Panel Code for Puzzle-2
*/

#include <WiFi.h>
#include "time.h"
#include <ESP32Servo.h>

/*********************
     Piezo Sensor
*********************/
int Puzzle = 0;
int checkZero = 0;
int piezoFlag = 0;
// these constants won't change:
const int knockSensorL = 35; // the left piezo is connected to analog pin 0
const int knockSensorR = 34; //the right piezo is connected to analog pin 1
const int threshold = 300;  // threshold value to decide when the detected sound is a knock or not
int count1 = 0;
int count2 = 0;
// these variables will change:
int sensorReadingL = 0;
int sensorReadingR = 0;// variable to store the value read from the sensor pin



/*********************
        Servo
*********************/
Servo servo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 21;
int servoFlag = 0;



/*********************
  Other Declarations
*********************/
//Local Time Variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int hourNow, CurrentHour, UnitHour, TenHour;

//Timer Declaration
unsigned long int progTimer;
int fullTime=60000;    //Time after which ESP32 restarts
int puzzleTime=30000;  //Time limit for the puzzle to be solved

// Pin of internal LED of the ESP32
#define LED_PIN 2

// Speed of Serial Communication
#define SERIAL_COMSPEED 9600

// Credentials for network SSID and PWD
#define SSID "dlink 46"
#define PWD "Seepark46"

// Device can be found on the network using this name
#define NAME "Puzzle_2"


void setup() {
  // Initialize the Serial Port
  Serial.begin(SERIAL_COMSPEED);

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
          Servos
  *********************/
  servo.setPeriodHertz(50);  // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
  servo.write(160);  //set to default position 

  /*********************
        Others
  *********************/ 
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getCurrentHour(); 
  hourNow = getCurrentHour();
  UnitHour = hourNow%10;
  TenHour =(hourNow/10)%10;
}
 
void loop(){
  
  /*********************
     Piezo Sensor
  *********************/
  

   sensorReadingL = analogRead(knockSensorL);
   sensorReadingR = analogRead(knockSensorR);
   
   if(UnitHour==0) checkZero=1;
   
   if ((sensorReadingL >= threshold)&&(Puzzle == 0)){ 
     count1++;
     Serial.println("Knock1");  
     delay(150);       
     if (count1 >= TenHour) {
      Serial.println("knock 1 routine done");
      Puzzle = 1;
     }
   }
      
   else if ((sensorReadingR >= threshold)&&(Puzzle == 1)&&(UnitHour>0)){   
     count2++;  
     Serial.println("Knock2");
     delay(150);   
     if (count2 >= UnitHour) {
       Serial.println("knock 2 routine done");
       Puzzle = 2;
      }
    }
     
    else if (Puzzle == 2 ||(Puzzle == 1 && checkZero == 1)){  
    /*********************
             Servo
    *********************/
    for (pos = 160; pos >= 10; pos -= 1) { // goes from 0 degrees to 120 degrees in steps of 1 degree
     servo.write(pos);    // tell servo to go to position in variable 'pos'
     delay(15);
      // waits 15ms for the servo to reach the position
    Puzzle == 3;
    } 
      Serial.println("Puzzle solved");
      servo.detach();
      count1=0;
      count2=0;
    }
  
    /*****************************
      Puzzle Reset after 15 mins
    *****************************/ 
    progTimer = millis();
    if(progTimer >= fullTime){
      esp_restart();
    }
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
