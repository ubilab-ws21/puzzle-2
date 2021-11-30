# Team 2: Lights Will Guide You

## Contents
- [Story Overview](#1)
- [The Puzzle](#2)
- [Use Case Scenario](#3)
- [System Overview](#4)
- [Project Timeline](#5)
- [Hardware Overview](#6)
- [References](#7)

## Story Overview <a name="1"></a>
A Solar strom themed Escape Room where players have to solve puzzles to get out of it. In the year 2032, the world revolving around automation and high-end technology. One fine day, a Solar Storm hits the city and all the servers and digital systems starts to fail there is a power breakdown. The players are stuck inside a server room having limited emergency backup. The goal is to upload the software to the server in order to get things back in place.   

## The Puzzle <a name="2"></a>
We are building a puzzle for the server room of the Solar Storm themed Escape Room for Ubiquitous Laboratory Area-079 Escape Room. The puzzle is designed in regard to access the power delivery system which will be powering the necessary server which contains the corrupted software. The emergency power backup can power up only a single server and if wrong switches are turned on the backup will drain out. 

The power switches are protected by a tap-tap pattern for which the players have to solve a riddle. If the pattern is wrong for 2 consecutive times, the switch board gets locked for 2 minutes. Once you get the pattern right you get access to the power switches. But....

There are lots of switches and you need to turn on few of them. One wrong switch can drain out the emergency backup. The order of the switches which needs to be turned on will be coded and be given using RGB LED Strips. You get the right sequence of switches, the server turns on.

## Use Case Scenario <a name="3"></a>
<img src="https://github.com/ubilab-ws21/puzzle-2/blob/main/Documentation/use_case_scenario.jpg" height="300px" width="650px">

## System Overview <a name="4"></a>
<img src="https://github.com/ubilab-ws21/puzzle-2/blob/main/Documentation/system_overview.jpg" width="650px">

## Project Timeline <a name="5"></a>
- [x] Figuring the puzzle according to the story
- [x] Creating a Bill of Materials (BOM)
- [x] Starting with ESP32 Programming and Basics of MQTT 
- [x] First tour of Escape Room
- [ ] Checking all components
- [ ] Preparing the circuit
- [ ] Program the ESP Board
- [ ] ...
- [ ] Testing and Bug Fixing
- [ ] Final run and presentation

## Hardware Overview <a name="6"></a>
- ESP-32 PICO-D4 Microcontroller
- Servo Motor
- Piezo Buzzer
- Toggle Switches
- 8x5050 RGB LED Strip

For more details, check [Bill of Materials](https://github.com/ubilab-ws21/puzzle-2/blob/main/Bill%20of%20Materials.xlsx).

## References <a name="7"></a>
-
