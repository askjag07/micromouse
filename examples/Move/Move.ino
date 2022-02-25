/* Moves forward one revolution with 3s breaks */

#include <Encoder.h>
#include <SoftwareSerial.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);

int numHoles = 0;
int encValue = 0;
int encState = 0;

void setup() {
  serial.begin(19200);
  Serial.begin(38400);
}

void loop() {
  // Control motor movement
  if (numHoles <= 135) {
    serial.write(127);
    serial.write(255);
  }
  else {
    serial.write(64);
    serial.write(192);
    delay(3000);
    numHoles = 0;
  }
  // Count number of slots the encoder sensed
  if (e.read() != encValue) {
    encValue = e.read();
    if (encValue == 0) {
      numHoles++;
      encValue = 0;
    }
  }
}
