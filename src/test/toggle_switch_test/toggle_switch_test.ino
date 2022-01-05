/* 
            Toggle Switch Test

  [Lights up internal LED when switch turned on]
  
*/

// Pin of toggle switch
#define SWITCH_PIN 21

// Pin of internal LED of the ESP32
#define LED_PIN 2

void setup() {
  pinMode(SWITCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if(digitalRead(SWITCH_PIN) == HIGH){
    digitalWrite(LED_PIN, LOW);
  }
  if(digitalRead(SWITCH_PIN) == LOW){
    digitalWrite(LED_PIN, HIGH);
  }
}
