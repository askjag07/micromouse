/* Moves forward one square. */

#include <Encoder.h>
#include <SoftwareSerial.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);

void setup()
{
  serial.begin(19200);
  turnLeft(1);
}

void loop()
{
}

void turnLeft(byte num)
{
  for (byte i = 0; i < num; i++)
  {
    byte numHoles = 0;
    bool encValue = 0;
    serial.write(127);
    serial.write(128);
    while (numHoles <= 100)
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
