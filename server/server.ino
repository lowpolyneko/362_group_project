#define startingLives 5
#define resetButtonPin 12
#define maxRight 5
#define maxLeft 1
#define resetDely 2000

#include <LedControl.h>

#define DIN 8
#define CS  9
#define CLK 10

LedControl lc = LedControl(DIN, CLK, CS,0);

int playerOneLives;
int playerTwoLives;
int playerThreeLives;
bool playerOneAlive = true;
bool playerTwoAlive = true;
bool playerThreeAlive = true;
int playerOneBlockerPos = 3;
int playerTwoBlockerPos = 3;
int playerThreeBlockerPos = 3;
unsigned long nextBallUpdate = resetDely;
int ballX, ballY;
int ballXSpeed, ballYSpeed;


void resetBall()
{
  ballX = 3;
  ballY = 4;
  ballXSpeed = random(0,2);
  if(ballXSpeed == 0 ){
    ballXSpeed = -1;
  }
  ballYSpeed = random(0,2);
  if(ballYSpeed == 0 ){
    ballYSpeed = -1;
  }
}


void resetLives()
{
  playerOneLives = startingLives;
  playerTwoLives = startingLives;
  playerThreeLives = startingLives;
  playerOneAlive = true;
  playerTwoAlive = true;
  playerThreeAlive = true;
  Serial1.write(startingLives);
  Serial2.write(startingLives);
  Serial3.write(startingLives);
}


void updateDisplay()
{
  // set "player 4" or the empty row as full lights
  lc.setColumn(0, 7, 0b11111111);

  // player 1
  if (playerOneAlive)
    lc.setRow(0, 0, (0b11 << (6 - playerOneBlockerPos)) | 0b10000001);
  else
    lc.setRow(0, 0, 0b11111111);

  // player 2
  if (playerTwoAlive)
    lc.setColumn(0, 0, 0b11 << (6 - playerTwoBlockerPos) | 0b10000001);
  else
    lc.setColumn(0, 0, 0b11111111);

  // player 3
  if (playerThreeAlive)
    lc.setRow(0, 7, (0b11 << (6 - playerThreeBlockerPos)) | 0b10000001);
  else
    lc.setRow(0, 7, 0b11111111);

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

  randomSeed(analogRead(A5));

  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.begin(9600);
  
  pinMode(resetButtonPin, INPUT);

  delay(resetDely);
  resetBall();
  resetLives();
}


void loop()
{
  bool updFlag = false;

  if (Serial1.available() > 0) {
    switch(Serial1.read()) {
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

  if (Serial2.available() > 0) {
    switch(Serial2.read()) {
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

  if (Serial3.available() > 0) {
    switch(Serial3.read()) {
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

  // updates position
  if(millis() >= nextBallUpdate)
  {
    nextBallUpdate += map(analogRead(A0), 0, 1023, 500, 2000);
    
    //Update ball position
    ballY += ballYSpeed;
    ballX += ballXSpeed;


    //Check if ball is out of bounds
    if(ballY < 0)
    {
      playerTwoLives -= 1;
      Serial2.write(playerTwoLives);
      resetBall();
      if(playerTwoLives < 1)
        playerTwoAlive = false;
    }

    if(ballX < 0)
    {
      playerOneLives -= 1;
      Serial1.write(playerOneLives);
      resetBall();
      if(playerOneLives < 1)
        playerOneAlive = false;
    }
    else if(ballX > 7)
    {
      playerThreeLives -= 1;
      Serial3.write(playerThreeLives);
      resetBall();
      if(playerThreeLives < 1)
        playerThreeAlive = false;
    }

    //Reset ball if it hits a corner
    if(ballY <= 0 && (ballX >= 7 && ballY <= 0))
      resetBall();

    //Check if ball has collided
    if(ballY >= 6)
    {
      ballYSpeed = -ballYSpeed;
      ballXSpeed = random(-1, 2);
      ballY = 6;
    }
    else if(ballY <= 1)
      if (!playerTwoAlive || (ballX >= playerTwoBlockerPos && ballX <= playerTwoBlockerPos + 1)) //P2 is dead or the ball is colliding with P2's paddle
      {
        ballYSpeed = -ballYSpeed;
        ballXSpeed = random(-1, 2);
        ballY = 1;
      }

    if(ballX <= 1)
    {
      if((!playerOneAlive || (ballY >= playerOneBlockerPos && ballY <= playerOneBlockerPos + 1))) //P1 is dead or the ball is colliding with P1's paddle
      {
        ballXSpeed = -ballXSpeed;
        ballYSpeed = random(-1, 2);
        ballX = 1;
      }
    }
    else if(ballX >= 6)
    {
      if(!playerThreeAlive || (ballY >= playerThreeBlockerPos && ballY <= playerThreeBlockerPos + 1)) //P3 is dead or the ball is colliding with P3's paddle){
      {
        ballXSpeed = -ballXSpeed;
        ballYSpeed = random(-1, 2);
        ballX = 6;
      }
    }
    
    updFlag = true;
  }

  if (updFlag)
    updateDisplay();

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
