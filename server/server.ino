#define startingLives 5
#define ballUpdateFrequency 500
#define blockUpdateFrequency 1000
#define p1TX 2
#define p1RX 3
#define p2TX 4
#define p2RX 5
#define p3TX 6
#define p3RX 7
#define resetButtonPin 12
#define maxRight 5
#define maxLeft 1
#define resetDely 2000

#include <LedControl.h>
#include <SoftwareSerial.h>

#define DIN = 8;
#define CS  = 9;
#define CLK = 10;

LedControl lc = LedControl(DIN, CLK, CS,0);

SoftwareSerial p1Serial(p1RX, p1TX);
SoftwareSerial p2Serial(p2RX, p2TX);
SoftwareSerial p3Serial(p3RX, p3TX);

int playerOneLives;
int playerTwoLives;
int playerThreeLives;
bool playerOneAlive = true;
bool playerTwoAlive = true;
bool playerThreeAlive = true;
int playerOneBlockerPos = 3;
int playerTwoBlockerPos = 3;
int playerThreeBlockerPos = 3;
int nextBallUpdate = resetDely;
float ballX;
float ballY;
float ballXSpeed;
float ballYSpeed;


void resetBall()
{
  ballX = 3.5;
  ballY = 3.5;
  long LongBallDirection = random(0, 6283);
  double ballDirection = (double)LongBallDirection / 1000.0;
  ballXSpeed = cos(ballDirection);
  ballYSpeed = sin(ballDirection);
}


void resetLives()
{
  playerOneLives = startingLives;
  playerTwoLives = startingLives;
  playerThreeLives = startingLives;
  p1Serial.write(startingLives);
  p2Serial.write(startingLives);
  p3Serial.write(startingLives);
}


void updateDisplay()
{
  // "player 4" or the empty row
  lc.setColumn(0, 7, 0b11111111);

  // player 1
  if (playerOneAlive)
    lc.setRow(0, 0, (0b11 << playerOneBlockerPos) | 0b1);
  else
    lc.setRow(0, 0, 0b11111111);

  // player 2
  if (playerTwoAlive)
    lc.setColumn(0, 0, 0b11 << playerTwoBlockerPos);
  else
    lc.setColumn(0, 0, 0b11111111);

  // player 3
  if (playerThreeAlive)
    lc.setRow(0, 7, (0b11 << playerThreeBlockerPos) | 0b1);
  else
    lc.setRow(0, 0, 0b11111111);

  // ball (clear inner columns first)
  for (int x = 1; x < 7; ++x)
    for (int y = 1; y < 7; ++y)
      lc.setLed(0, x, y, false);

  lc.setLed(0, ballX, ballY, true);
}


void setup()
{
  //TODO set up 8x8 LED matrix
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);

  for (int i = 2; i <= 7; i++)
  {
    pinMode(i, (i % 2) ? INPUT : OUTPUT);
  }

  pinMode(resetButtonPin, INPUT);
  resetBall();
  resetLives();

  p1Serial.begin(9600);
  p2Serial.begin(9600);
  p3Serial.begin(9600);
}


void loop()
{
  bool updFlag = false;

  if (p1Serial.available() > 0) {
    switch(p1Serial.read()) {
      case 'r':
        if(playerOneBlockerPos < maxRight)
          playerOneBlockerPos += 1;
        break;
      case 'l':
        if(playerOneBlockerPos > maxLeft)
          playerOneBlockerPos -= 1;
    }

    updFlag = true;
  }

  if (p2Serial.available() > 0) {
    switch(p2Serial.read()) {
      case 'r':
        if(playerTwoBlockerPos < maxRight)
          playerTwoBlockerPos += 1;
        break;
      case 'l':
        if(playerTwoBlockerPos > maxLeft)
          playerTwoBlockerPos -= 1;
    }

    updFlag = true;
  }

  if (p3Serial.available() > 0) {
    switch(p3Serial.read()) {
      case 'r':
        if(playerThreeBlockerPos < maxRight)
          playerThreeBlockerPos += 1;
        break;
      case 'l':
        if(playerThreeBlockerPos > maxLeft)
          playerThreeBlockerPos -= 1;
    }

    updFlag = true;
  }

  if (updFlag)
    updateDisplay();

  if(millis() >= nextBallUpdate)
  {
    nextBallUpdate += ballUpdateFrequency;
    //Update ball position
    ballX += ballYSpeed;
    ballY += ballXSpeed;


    //Check if ball is out of bounds
    if(ballY < 0)
    {
      playerTwoLives -= 1;
      p2Serial.write(playerTwoLives);
      resetBall();
      if(playerTwoLives < 1)
        playerTwoAlive = false;
    }


    if(ballX < 0)
    {
      playerOneLives -= 1;
      p1Serial.write(playerOneLives);
      resetBall();
      if(playerOneLives < 1)
        playerOneAlive = false;
    }
    else if(ballX > 7)
    {
      playerThreeLives -= 1;
      p3Serial.write(playerThreeLives);
      resetBall();
      if(playerThreeLives < 1)
        playerThreeAlive = false;
    }


    //Check if ball has collided
    if((int)ballY > 6)
    {
      ballYSpeed = -ballYSpeed;
      ballY = 6;
    }
    else if((int)ballY < 1 &&
        (!playerTwoAlive ||
         ((int)ballX > playerTwoBlockerPos && (int)ballX < playerTwoBlockerPos + 1)))
    {
      ballYSpeed = -ballYSpeed;
      ballY = 1;
    }
    if((int)ballX < 1 &&
        (!playerOneAlive ||
         ((int)ballY > playerOneBlockerPos && (int)ballY < playerOneBlockerPos + 1)))
    {
      ballXSpeed = -ballXSpeed;
      ballX = 1;
    }
    else if((int)ballX > 6 &&
        (!playerThreeAlive ||
         ((int)ballY > playerThreeBlockerPos && (int)ballY < playerThreeBlockerPos + 1)))
    {
      ballXSpeed = -ballXSpeed;
      ballX = 6;
    }


    updateDisplay();
  }

  if(digitalRead(resetButtonPin) == HIGH)
  {
    delay(resetDely);
    nextBallUpdate = millis() + resetDely;

    playerOneBlockerPos = 3;
    playerTwoBlockerPos = 3;
    playerThreeBlockerPos = 3;
    resetLives();
    resetBall();
  }
}

// vim: set ts=2 sw=2 expandtab:
