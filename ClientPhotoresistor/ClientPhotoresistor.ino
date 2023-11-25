//Reference for ultrasonic sensor
//https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1

//Library used for displaying numbers on seven segment display
//https://www.arduino.cc/reference/en/libraries/seven-segment-library/
#include <SegmentDisplay.h>

#define UPDATE_FREQUENCY 250
#define LEFT_MOVE_PIN 2
#define RIGHT_MOVE_PIN 3
#define READ_PIN A0
#define LED_PIN 12

int RIGHT_CUTOFF;
int LEFT_CUTOFF;

int nextUpdate = UPDATE_FREQUENCY;
int currentLives = 0;
SegmentDisplay segmentDisplay(4, 5, 10, 11, 9, 8, 7, 6);

void setup() {
  Serial.begin(9600);
  int initialRead = analogRead(READ_PIN);
  RIGHT_CUTOFF = initialRead - 100;
  LEFT_CUTOFF = initialRead + 100;
}

void loop() {

  if(currentLives > 0)
  {
    if(millis() >= nextUpdate)
    {
      nextUpdate += UPDATE_FREQUENCY;
      //Read input from pressure sensor
      int userInput = analogRead(A0);

      if(userInput < RIGHT_CUTOFF)
        Serial.write('r');
      else if(userInput > LEFT_CUTOFF)
        Serial.write('l');
    }
  }

  if(Serial.available() > 0)
  {
    int newLives = Serial.read();
    currentLives = newLives;
    segmentDisplay.displayHex(newLives, false);
    if(currentLives == 0){
      digitalWrite(LED_PIN, LOW);
    }else{
      digitalWrite(LED_PIN, HIGH);
    }
  }
}