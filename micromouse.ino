/* Moves forward one square. */

#include <Encoder.h>
#include <SoftwareSerial.h>
#include <HCSR04.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);
HCSR04 hc(5, 6);

void setup()
{
  Serial.begin(38400);
  serial.begin(19200);
  moveSquares(getSquares());
}

void loop()
{
}

int getSquares()
{
  return (int)round(hc.dist() / 25.4);
}

void moveSquares(byte num)
{
  for (byte i = 0; i < num; i++)
  {
    int numHoles = 0;
    bool encValue = 0;
    serial.write(127);
    serial.write(255);
    while (numHoles <= 300)
      if (e.read() != encValue)
      {
        encValue = e.read();
        if (encValue == 0)
        {
          numHoles++;
          encValue = 0;
        }
      }
    serial.write(64);
    serial.write(192);
  }
}