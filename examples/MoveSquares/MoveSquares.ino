/* Moves forward one square */

#include <SoftwareSerial.h>
#include <Encoder.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);

void setup()
{
    serial.begin(19200);
    moveSquares(1);
}

void loop()
{
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
