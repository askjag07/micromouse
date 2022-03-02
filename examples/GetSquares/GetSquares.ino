/* Gets number of squares from sensor to wall. */

#include <HCSR04.h>

HCSR04 hc(5, 6);

void setup()
{
  Serial.begin(38400);
  Serial.println(getDist());
}

void loop()
{
}

int getDist()
{
  return (int)round(hc.dist() / 25.4) * 10;
}
