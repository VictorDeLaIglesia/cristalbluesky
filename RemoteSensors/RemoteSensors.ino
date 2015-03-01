#include "BTComLib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

const uint8_t Rx = 8;
const uint8_t Tx = 9;
const uint8_t OneWPin = 1;

BTRemoteSensors BTSensors(8, 9, 1);

void setup()
{
	BTSensors.begin();
}

void loop()
{
	BTSensors.loop();
}