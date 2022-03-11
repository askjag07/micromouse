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

void setup()
{
  serial.begin(19200);
  while (true)
  {
    moveSquares(getSquares(1), false);
    if (getSquares(0) > 0)
      turn(true);
    else if (getSquares(2) > 0)
      turn(false);
    else
      break;
  }
}

void loop()
{
}

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
