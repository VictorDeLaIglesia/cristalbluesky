// BTComLib.h

#ifndef _BTCOMLIB_h
#define _BTCOMLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define RDD 1
#define WRD 2
#define RDA 3
#define WRA 4
#define TEM 5
#define LCD 6

#define BRDD 101
#define BWRD 102
#define BRDA 103
#define BWRA 104
#define BTEM 105
#define BLCD 106
#define BERR 100

#define HIGHVAL "HIGHVAL"
#define LOWVAL "LOWVAL"

#define RX 8
#define TX 9
#define OWP 3
#define SERIALSPEED 9600

int const BUFSIZE = 40;

class BTRemoteSensors
{
 protected:

 public:
	BTRemoteSensors();
	BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin);
	BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin, long serialSpeed);
	void begin();
	void loop();
	bool conected();
	float getTemperature();
	SoftwareSerial  *_btSerial;
	char * processRequest(char *  command);
	bool getDigital(int pin);
	int getAnalog(int pin);
	
 private:
	OneWire* _oneWire;
	DallasTemperature* _sensors;
	uint8_t _Rx; 
	uint8_t _Tx;
	uint8_t _OneWPin;
	long _serialSpeed;
	int _index = 0;
	char command[BUFSIZE];
	bool digitalData[13];
	int analogData[6];
};

#endif

