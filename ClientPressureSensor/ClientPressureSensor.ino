//Reference for ultrasonic sensor
//https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1

//Library used for displaying numbers on seven segment display
//https://www.arduino.cc/reference/en/libraries/seven-segment-library/
#include <SegmentDisplay.h>

#define LEFT_MOVE_PIN 2
#define RIGHT_MOVE_PIN 3
#define READ_PIN A0
#define LED_PIN 12
//These values will be different on other arduinos
#define RIGHT_CUTOFF 25
#define LEFT_CUTOFF 100
#define THIS_ARDUINO_CODE 1

int currentLives = 0;
SegmentDisplay segmentDisplay(4, 5, 10, 11, 9, 8, 7, 6);

void setup() {
  Serial.begin(9600);
}

void loop() {

  if(currentLives > -1)
  {
    //Read input from pressure sensor
    int userInput = analogRead(A0);

    if(userInput < RIGHT_CUTOFF)
      digitalWrite(RIGHT_MOVE_PIN, HIGH);
    else
      digitalWrite(RIGHT_MOVE_PIN, LOW);

    if(userInput > LEFT_CUTOFF)
      digitalWrite(LEFT_MOVE_PIN, HIGH);
    else
      digitalWrite(LEFT_MOVE_PIN, LOW);
  }
  else{
    digitalWrite(LEFT_MOVE_PIN, LOW);
    digitalWrite(RIGHT_MOVE_PIN, LOW);
  }

  if(Serial.available() > 0)
    {
    int serialInput = Serial.read();
    int arduinoCode = serialInput / 16;
    int newLives = serialInput % 16;
    //Note THIS_ARDUINO_CODE is different for each client
    if(arduinoCode == THIS_ARDUINO_CODE)
    {
      currentLives = newLives;
      segmentDisplay.displayHex(newLives, false);
      if(currentLives == 0){
        digitalWrite(LED_PIN, LOW);
      }else{
        digitalWrite(LED_PIN, HIGH);
      }
    }
  }
}
