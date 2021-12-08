#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Pin of internal LED of the ESP32
#define LED_PIN 2
// Pin you connect a button to
#define BUTTON_PIN 5
// Debounce delay for the button
#define DEBOUNCE_DELAY 100

// Speed of Serial Communication
#define SERIAL_COMSPEED 115200

// Credentials for network SSID and PWD
#define SSID "TP-Link_703C"
#define PWD "Freiburg@2020"

// Device can be found on the network using this name
#define NAME "Puzzle_2"

// PWD for OTA
#define OTA_PWD "sss21"

// TCP Client connecting to this server
// IP or DNS name of TCP Server
#define SERVER_IP "192.168.1.113"
// Port of server, should be >= 2000
#define SERVER_PORT 2000
WiFiClient client;

// MQTT server or DNS
#define MQTT_SERVER_IP "192.168.1.113"
// Standard port for MQTT
#define MQTT_PORT 1883
// RAW TCP client and pubsub class using it
WiFiClient mqttClient;
PubSubClient mqtt(mqttClient);

// Global cstring for message building
#define MSG_SIZE 50
char msg[MSG_SIZE] = {'\0'};

// Enum and global variable for puzzle state
enum PuzzleState {
  idle = 0,
  active = 1,
  solved = 2,
};
PuzzleState puzzleState = idle;

// JSON dict of given size
StaticJsonDocument<2 * MSG_SIZE> dict;

// Setup function that runs once upon startup or reboot
void setup() {
  // Initialize the Serial Port
  Serial.begin(SERIAL_COMSPEED);

  /*********************
     IO setup
   *********************/
  // LED pin is output
  pinMode(LED_PIN, OUTPUT);
  // Button pin is input
  // INPUT_PULLUP means driven high by an internal pullup
  // resistor. The pin can be driven low with external connection
  pinMode(BUTTON_PIN, INPUT_PULLUP);

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
     multicast DNS
   *********************/
  // Set name of this device
  if (!MDNS.begin(NAME)) {
    Serial.println("Setting up MDNS responder!");
  }
  // The service this device hosts (example)
  MDNS.addService("_escape", "_tcp", 2000);

  /*************************
     Connect to TCP server
   *************************/
  if (client.connect(SERVER_IP, SERVER_PORT)) {
    Serial.println("Connected to server");
  } else {
    Serial.println("Cannot connect to server");
  }

  // OTA setup in separate function
  setupOTA();


  /*************************
     MQTT Setup
   *************************/
  // Set IP and port of broker
  mqtt.setServer(MQTT_SERVER_IP, MQTT_PORT);
  // Set CB function to be called for all subscriptions
  mqtt.setCallback(mqttCallback);
  // Connect to broker using a unique ID (here the name)
  if (mqtt.connect(NAME)) {
    Serial.println("Connected to MQTT server");
    // Subscribe to a certain topic
    mqtt.subscribe("myPuzzle/set");
  } else {
    Serial.println("Cannot connect to MQTT server");
  }

  // Indicate end of setup
  Serial.println("Setup done!");
}

void loop() {
  // Needs to be called to check for external update request
  ArduinoOTA.handle();
  // Checks for MQTT subscriptions
  mqtt.loop();

  // Get current state of button
  bool pressed = digitalRead(BUTTON_PIN);
  // If button was pressed
  if (pressed) {
    // Wait to prevent multipresses
    delay(DEBOUNCE_DELAY);
    // Optionally wait for release (yield required for watchdog)
    while (!digitalRead(BUTTON_PIN)) {
      yield();
    }
    // Change state from idle to active afterwards cycle between
    // solved and active
    if (puzzleState == idle) {
      puzzleActive();
    } else if (puzzleState == active) {
      puzzleSolved();
    } else {
      puzzleActive();
    }
  }

  // If at least one byte is available over serial
  if (Serial.available()) {
    handleStream(&Serial);
  }

  // If at least one byte is available over TCP
  if (client.available()) {
    handleStream(&client);
  }
}


/*
   Handles incoming data of a stream
   This can be any stream compliant class such as
   Serial, WiFiClient (TCP or UDP), File, etc.
*/
void handleStream(Stream * getter) {
  // Copy incoming data into msg cstring until newline is received
  snprintf(msg, MSG_SIZE, "%s", getter->readStringUntil('\n').c_str());
  // Some systems also send a carrige return, we want to strip it off
  for (size_t i = 0; i < MSG_SIZE; i++) {
    if (msg[i] == '\r') {
      msg[i] = '\0';
      break;
    }
  }
  // Handle given Msg
  const char * returnMsg = handleMsg(msg);
  // Write something back to stream only if wanted
  if (returnMsg[0] != '\0') {
    getter->println(returnMsg);
  }
}

/*
   Handle a received message, may return some answer.
*/
const char * handleMsg(const char * msg) {
  // strcmp returns zero on a match
  if (strcmp(msg, "solved") == 0) {
    puzzleSolved();
  } else if (strcmp(msg, "active") == 0) {
    puzzleActive();
  } else if (strcmp(msg, "idle") == 0) {
    puzzleIdle();
  } else {
    return "Unknown command";
  }
  return "";
}

/*
   Puzzle changes to idle state
*/
void puzzleIdle() {
  puzzleState = idle;
  // Code to idle puzzle ...
  puzzleStateChanged();
}

/*
   Puzzle changes to solved state
*/
void puzzleSolved() {
  puzzleState = solved;
  // Code to set puzzle into solved state ...
  puzzleStateChanged();
}

/*
   Puzzle changes to active state
*/
void puzzleActive() {
  puzzleState = active;
  // Code to set puzzle into active state ...
  puzzleStateChanged();
}

/*
   Indicate to others that the state of the
   puzzle changed
*/
void puzzleStateChanged() {
  Serial.printf("Puzzle state changed to: %s\n", puzzleStateToStr(puzzleState));

  // Convert puzzlestate into JSON dict
  JsonObject obj = dict.to<JsonObject>();
  obj.clear();
  obj["state"] = puzzleStateToStr(puzzleState);
  // Serialize into msg cstring
  serializeJson(obj, msg);

  // If the TCP client is connected to the server, send JSON
  if (client.connected()) {
    client.println(msg);
  }
  // If the MQTT client is connected to broker, send JSON
  if (mqtt.connected()) {
    // Specify if msg should be retained
    bool retained = true;
    // Publish msg under given topic
    mqtt.publish("myPuzzle/status", "test"); //"test", retained);                                                                                                                                                                                                                                                                                                                                           , retained);
  }

  // Set LED depending on Puzzle state
  if (puzzleState == solved) {
    digitalWrite(LED_PIN, HIGH);
  } else if (puzzleState == idle) {
    digitalWrite(LED_PIN, LOW);
  }
}

/*
   Convert the ENUM to a string
*/
const char * puzzleStateToStr(PuzzleState puzzle) {
  switch (puzzle) {
    case active: return "active";
    case idle: return "idle";
    case solved: return "solved";
    default: return "Unknown state";
  }
}

/*
   Setup OTA Updates
*/
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

/*
 * Callback for MQTT messages on subscribed topics
 */
void mqttCallback(char* topic, byte* message, unsigned int length) {
  // Convert message as byte array to cstring
  unsigned int len = min((int)length, (int)(MSG_SIZE - 1));
  memcpy(&msg[0], message, len);
  msg[len] = '\0';
  // Print over serial
  Serial.printf("MQTT msg on topic: %s: %s\n", topic, &msg);

  // Try to deserialize the JSON msg
  DeserializationError error = deserializeJson(dict, msg);
  // Test if parsing succeeds.
  if (error) {
    Serial.println("error deserializing the msg");
  } else {
    Serial.println("Msg is of valid JSON format");
    // Try to extract new state
    const char* newState = dict["state"];
    // If key exists this is not NULL
    if (newState) {
      handleMsg(newState);
    }
  }
}
