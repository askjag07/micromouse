/* Gets number of squares in front of sensor */

#include <HCSR04.h>

HCSR04 front(5, 6);

void setup()
{
  Serial.begin(38400);
  Serial.println(getSquares());
}

void loop()
{
}

byte getSquares()
{
  return (byte)round(front.dist() / 25.4);
}
