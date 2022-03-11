/* Moves forward one revolution with 3s breaks */

#include <SoftwareSerial.h>
#include <Encoder.h>
#include <HCSR04.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);

HCSR04 left(3, 4);
HCSR04 front(5, 6);
HCSR04 right(9, 10);
HCSR04 hc[] = {left, front, right};

struct squares[10][10];
byte row = 0, col = 0;
// 0 is up, 1 is right, 2 is down, 3 is left
byte dir = 1;

// sets back wall of first square to closed
squares[0][0].s2 = 1;

void setup()
{
  serial.begin(19200);
  
}

void loop()
{
  setSquare();

}

void setSquare() {
  if(dir == 0) {
    if(getSquares(0) > 0) squares[row][col].left = 1;
    else squares[row][col].left = 0;
    if(getSquares(1) > 0) squares[row][col].up = 1;
    else squares[row][col].up = 0;
    if(getSquares(2) > 0) squares[row][col].right = 1;
    else squares[row][col].right = 0;
  }
  else if(dir == 1) {
    if(getSquares(0) > 0) squares[row][col].up = 1;
    else squares[row][col].up = 0;
    if(getSquares(1) > 0) squares[row][col].right = 1;
    else squares[row][col].right = 0;
    if(getSquares(2) > 0) squares[row][col].down = 1;
    else squares[row][col].down = 0;
  }
  else if(dir == 2) {
    if(getSquares(0) > 0) squares[row][col].right = 1;
    else squares[row][col].right = 0;
    if(getSquares(1) > 0) squares[row][col].down = 1;
    else squares[row][col].down = 0;
    if(getSquares(2) > 0) squares[row][col].left = 1;
    else squares[row][col].left = 0;
  }
  else {
    if(getSquares(0) > 0) squares[row][col].down = 1;
    else squares[row][col].down = 0;
    if(getSquares(1) > 0) squares[row][col].left = 1;
    else squares[row][col].left = 0;
    if(getSquares(2) > 0) squares[row][col].up = 1;
    else squares[row][col].up = 0;
  }
  /*
  if(row > 0) squares[row-1][col].down = squares[row][col].up;
  if(row < 9) squares[row+1][col].down = squares[row][col].up;
  if(col > 0) squares[row-1][col].down = squares[row][col].up;
  if(col < 9) squares[row-1][col].down = squares[row][col].up;
  */
}

pair<byte,byte> checkWin() {
  for(byte r = 0; r < 9; r++) {
    for(byte c = 0; c < 9; c++) {
      if(squares[r][c].right == 0 && squares[r][c].down == 0 && squares[r+1][c].up == 0 && squares[r+1][c].right == 0 && 
          squares[r][c+1].left == 0 && squares[r][c+1].down == 0 && squares[r+1][c+1].up == 0 && squares[r+1][c+1].left == 0) {
        return make_pair(r,c);
      }
    }
  } 
}

// 0 is left, 1 is front, 2 is right
byte getSquares(byte d)
{
  return rnd(hc[d].dist() / 25.4);
}

void moveSquares(byte squares, bool b)
{
  for (byte i = 0; i < squares; i++)
  {
    int numHoles = 0;
    byte encValue = 0;

    serial.write(b ? 1 : 127);
    serial.write(b ? 128 : 255);
    while (numHoles <= (b ? 50 : 303))
    {
      if (e.read() != encValue)
      {
        encValue = e.read();
        if (encValue == 0)
        {
          numHoles++;
          encValue = 0;
        }
      }
    }
    serial.write(64);
    serial.write(192);
    delay(1000);
  }
}

void turn(bool left)
{
  int numHoles = 0;
  byte encValue = 0;

  if (left)
  {
    serial.write(127);
    serial.write(128);
  }
  else
  {
    serial.write(1);
    serial.write(255);
  }
  while (numHoles <= (left ? 95 : 80))
  {
    if (e.read() != encValue)
    {
      encValue = e.read();
      if (encValue == 0)
      {
        numHoles++;
        encValue = 0;
      }
    }
  }
  serial.write(64);
  serial.write(192);
  delay(1000);
  moveSquares(1, true);
}

int rnd(float n)
{
  if (n - (byte)n < 0.5)
    return (int)n;
  else
    return (int)n + 1;
}

// top is side0, go clockwise
struct square {
  // 1 is wall, 0 is no wall, 2 is don't know
  byte up = 2, right = 2, down = 2, left = 2;
  
};