//Reference for ultrasonic sensor
//https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1

//Library used for displaying numbers on seven segment display
//https://www.arduino.cc/reference/en/libraries/seven-segment-library/
#include <SegmentDisplay.h>

#define LEFT_MOVE_PIN 2
#define RIGHT_MOVE_PIN 3
#define LED_PIN 11
#define TRIG_PIN 12
#define ECHO_PIN 13
//These values will be different on other arduinos
#define RIGHT_CUTOFF 300
#define LEFT_CUTOFF 600
#define THIS_ARDUINO_CODE 0

int currentLives = 0;
SegmentDisplay segmentDisplay(4, 5, 10, -1, 9, 8, 7, 6);

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);

}

void loop() {

  if(currentLives > -1)
  {
    //Read input from ultrasonic sensor 
    //On other clients this can be replaced by a simple analog read
    digitalWrite(TRIG_PIN, HIGH);
    delay(10);
    digitalWrite(TRIG_PIN, LOW);
    int userInput = pulseIn(ECHO_PIN, HIGH);

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

