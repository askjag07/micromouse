/* Gets number of squares in front of sensor */

#include <HCSR04.h>

HCSR04[] hc = [ left(5, 6), front(7, 8), right(9, 10) ];

void setup()
{
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
