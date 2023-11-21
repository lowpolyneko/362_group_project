#define playerOneCode 0
#define playerTwoCode 1
#define playerThreeCode 2
#define startingLives 5
#define ballUpdateFrequency 500
#define blockUpdateFrequency 1000
#define playerOneLeftMovePin 2
#define playerOneRightMovePin 3
#define playerTwoLeftMovePin 4
#define playerTwoRightMovePin 5
#define playerThreeLeftMovePin 6
#define playerThreeRightMovePin 7
#define resetButtonPin 12
#define maxRight 5
#define maxLeft 1
#define resetDely 2000

#include <LedControl.h>

int DIN = 8;
int CS  = 9;
int CLK = 10;

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
int nextBallUpdate = resetDely;
int nextBlockerUpdate = resetDely;
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
  Serial.write(16 * playerOneCode + startingLives);
  Serial.write(16 * playerTwoCode + startingLives);
  Serial.write(16 * playerThreeCode + startingLives);
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
    pinMode(i, INPUT);
  }
  pinMode(resetButtonPin, INPUT);
  resetBall();
  resetLives();
  Serial.begin(9600);
}


void loop()
{
  if(millis() >= nextBlockerUpdate)
  {
  nextBlockerUpdate += blockUpdateFrequency;
  //Update blocker position
  if(digitalRead(playerOneLeftMovePin) == HIGH && playerOneBlockerPos < maxRight)
    playerOneBlockerPos += 1;
  if(digitalRead(playerOneRightMovePin) == HIGH && playerOneBlockerPos > maxLeft)
    playerOneBlockerPos -= 1;
  if(digitalRead(playerTwoLeftMovePin) == HIGH && playerTwoBlockerPos < maxRight)
    playerTwoBlockerPos += 1;
  if(digitalRead(playerTwoRightMovePin) == HIGH && playerTwoBlockerPos > maxLeft)
    playerTwoBlockerPos -= 1;
  if(digitalRead(playerThreeLeftMovePin) == HIGH && playerThreeBlockerPos < maxRight)
    playerThreeBlockerPos += 1;
  if(digitalRead(playerThreeRightMovePin) == HIGH && playerThreeBlockerPos > maxLeft)
    playerThreeBlockerPos -= 1;

    updateDisplay();
  }


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
      Serial.write(16 * playerTwoCode + playerTwoLives);
      resetBall();
      if(playerTwoLives < 1)
        playerTwoAlive = false;
    }


    if(ballX < 0)
    {
      playerOneLives -= 1;
      Serial.write(16 * playerOneCode + playerOneLives);
      resetBall();
      if(playerOneLives < 1)
        playerOneAlive = false;
    }
    else if(ballX > 7)
    {
      playerThreeLives -= 1;
      Serial.write(16 * playerThreeCode + playerThreeLives);
      resetBall();
      if(playerThreeLives < 1)
        playerThreeAlive = false;
    }


    //Check if ball has collided
    if(ballY > 6)
    {
      ballYSpeed = -ballYSpeed;
      ballY -= 1;
    }
    else if(ballY < 1 &&
           (!playerTwoAlive ||
           (ballX > playerTwoBlockerPos && ballX < playerTwoBlockerPos + 1)))
    {
      ballYSpeed = -ballYSpeed;
      ballY += 1;
    }
    if(ballX < 1 &&
      (!playerOneAlive ||
      (ballY > playerOneBlockerPos && ballY < playerOneBlockerPos + 1)))
    {
      ballXSpeed = -ballXSpeed;
      ballX += 1;
    }
    else if(ballX > 6 &&
           (!playerThreeAlive ||
           (ballY > playerThreeBlockerPos && ballY < playerThreeBlockerPos + 1)))
    {
      ballXSpeed = -ballXSpeed;
      ballX -= 1;
    }


    updateDisplay();
  }
  
  if(digitalRead(resetButtonPin) == HIGH)
  {
    delay(resetDely);
    nextBallUpdate = millis() + resetDely;
    nextBlockerUpdate = nextBallUpdate;
    
    playerOneBlockerPos = 3;
    playerTwoBlockerPos = 3;
    playerThreeBlockerPos = 3;
    resetLives();
    resetBall();
  }
}
