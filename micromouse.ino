/* Moves forward one revolution with 3s breaks */

#include <SoftwareSerial.h>
#include <Encoder.h>
#include <HCSR04.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);
HCSR04[] hc = [ left(5, 6), front(7, 8), right(9, 10) ];

void setup()
{
  serial.begin(19200);
  Serial.begin(38400);
  Serial.println(getSquares());
}

void loop()
{
}

byte getSquares(byte d)
{
  return (byte)round(hc[d].dist() / 25.4);
}

void moveSquares(byte squares)
{
  for (byte i = 0; i < squares; i++)
  {
    int numHoles = 0;
    byte encValue = 0;

    serial.write(127);
    serial.write(255);
    while (numHoles <= 300)
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
  while (numHoles <= 100)
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
}
