/* Gets number of squares in front of sensor */

#include <HCSR04.h>

HCSR04 left(3, 4);
HCSR04 front(5, 6);
HCSR04 right(9, 10);
HCSR04 hc[] = {left, front, right};

void setup()
{
  Serial.begin(38400);
  Serial.println(getSquares(1));
}

void loop()
{
}

byte getSquares(byte d)
{
  return (byte)round(hc[d].dist() / 25.4);
}
