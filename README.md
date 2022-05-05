# Team 2: Time will guide you!

## Contents
- [Story Overview](#1)
- [The Puzzle](#2)
- [Use Case Scenario](#3)
- [System Overview](#4)
- [Project Workflow](#5)
- [Hardware Overview](#6)
  - [Electronic Components](#7)
  - [The Box](#8) 
- [Software Overview](#9)
  - [Testing Individual Modules](#10)
  - [Building the Main Program](#11)
  - [Configuring OTA](#12)
  - [Establishing MQTT Communication](#13) 
- [References](#14)

## Story Overview <a name="1"></a>
A Solar strom themed Escape Room where players have to solve puzzles to get out of it. In the year 2032, the world revolving around automation and high-end technology. One fine day, a Solar Storm hits the city and all the servers and digital systems starts to fail there is a power breakdown. The players are stuck inside a server room having limited emergency backup. The goal is to upload the software to the server in order to get things back in place.   

## The Puzzle <a name="2"></a>
We are building a puzzle for the server room of the Solar Storm themed Escape Room in the Ubiquitous Laboratory Area-079 Escape Room. The puzzle is designed in regard to access the power delivery system which will be powering the automated door to open the server room and power up the server which contains the corrupted software. The emergency power backup can power up only a single server and if wrong switches are turned on the backup will drain out. 

The power switches are protected by a tap-tap pattern for which the players have to be aware and take the hints from the previous puzzle. Once you get the pattern right you get access to the power switches. But, there are lots of switches and you need to turn on few of them. One wrong switch can drain out the emergency backup. The order of the switches which needs to be turned on will be given by the puzzle displayed in the OLED Display. You get the right sequence of switches, the server room door opens and the server turns on.

## Use Case Scenario <a name="3"></a>
<img src="https://github.com/ubilab-ws21/puzzle-2/blob/main/docs/system%20overview/use_case_scenario.jpg" height="500px" width="650px">

## System Overview <a name="4"></a>
<img src="https://github.com/ubilab-ws21/puzzle-2/blob/main/docs/system%20overview/system_overview.jpg" width="650px">

## Project Workflow <a name="5"></a>
- [x] Figuring the puzzle according to the story
- [x] Creating a Bill of Materials (BOM)
- [x] Starting with ESP32 Programming and Basics of MQTT 
- [x] First tour of Escape Room
- [x] Testing all components
- [x] Preparing the circuit
- [x] Program the ESP Board
- [x] Prepare 3D Model
- [x] Print 3D Model
- [x] Design the outer casing
- [x] Assemble the outer casing
- [x] Solder the circuit in PCB
- [x] Assembling all components 
- [x] Testing and Bug Fixing
- [x] Prototype Presentation
- [x] Final Tweaking and Testing
- [x] Final run and presentation

## Hardware Overview <a name="6"></a>

##### Electronic Components <a name="7"></a>
- ESP-32 Microcontroller
- Servo Motor
- Piezo Buzzer
- OLED Display
- Toggle Switches
- 8x5050 RGB LED Strip
- Printed Circuit Board
- Connecting Wires

For more details, check [Bill of Materials](https://github.com/ubilab-ws21/puzzle-2/tree/main/docs/BOM).

##### The Box <a name="8"></a>
The outer casing and the servo hinge was designed using 3D Modelling. The model files can be found [here](https://github.com/ubilab-ws21/puzzle-2/tree/main/design).

## Software Overview <a name="9"></a>

##### Testing Individual Modules <a name="10"></a>
Firstly, we tested all the components and modules individually for better integration of our project. Each modules were tested with Arduino for understanding the basic functionality and then tested with ESP32 to check the compatibility. The codes for testing can be found [here](https://github.com/ubilab-ws21/puzzle-2/tree/main/src/test).

##### Building the Main Program <a name="11"></a>
Then, we integrated all the modules and modified in a single program to be fired up in the ESP32. The program manages the worflow of the puzzle and maintains proper communication with the MQTT server. It is also programmed in such a way that the ESP32 can receives updates Over-the-air. The main code can be found [here](https://github.com/ubilab-ws21/puzzle-2/tree/main/src/main). 

##### Configuring OTA <a name="12"></a>
In order to receive updates Over-the-air, the below function has been called inside the setup loop. The function sets up the host name and password for the ESP32 in the newtork and updates the ESP32 if there is any update request. Also, remember to include the appropriate header file.
```
#include <ArduinoOTA.h>

void setup(){
  ....
  ....
  setupOTA();
}
void loop(){
  ...
  ...
  ArduinoOTA.handle();
}
```
##### Establishing MQTT Communication <a name="13"></a>
The puzzle workflow totally depends on successful interaction between the operator and the arcape. The operator controls whether the puzzle is to be turned  `on` or `off` depending on the progess of the other puzzles. The programs subscribes from the topic `2/esp` and checks whether the trigger is on/off and works accordingly. Once the puzzle is `on`, the program publishes that the puzzle is `active` in the topics, `2/esp` and `game/puzzle2` for the operator and arcape resp. to understand that the puzzle is active. Otherwise the state is retained to `inactive` state.

###### Communication Format
All messages exchanged between the server and the client is JSON schema: 

```json
{
  "method": "<method>",
  "state": "<state>",
  "data": "<data>"
}
```
where,

| Method  | State                            | Description                               |
| :------ | :------------------------------- | :---------------------------------------- |
| status  | inactive, active, solved         | Describes the current state of the puzzle |
| trigger | on, off                          | Contols the behaviour of the puzzle       |

More information regarding MQTT server and the communication protocols can be found [here](https://github.com/ubilab-ws21/operator/blob/master/README.md).<br/>
The JSON messages received by the server was parsed inside the function `decodeMessage()` and the messages sent to the server was created by the function `createJson()`.


## References <a name="14"></a>
- [Adafruit NeoPixel 5050](https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use)
- [Piezo Knock Sensor](https://www.arduino.cc/en/Tutorial/BuiltInExamples/Knock)
- [0.96" I2C OLED Display](https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/)
- [A Guide to ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
