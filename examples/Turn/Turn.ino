/* Turns ninety degrees left. */

#include <SoftwareSerial.h>
#include <Encoder.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);

void setup()
{
    serial.begin(19200);
    turn(true);
}

void loop()
{
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