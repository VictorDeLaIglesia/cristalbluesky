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

#define RX 8
#define TX 9
#define OWP 3
#define SERIALSPEED 4800

int const BUFSIZE = 40;

class BTRemoteSensors
{
 protected:
	char * processRequest(char *  command);

 public:
	BTRemoteSensors();
	BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin);
	BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin, int serialSpeed);
	void begin();
	void loop();
	bool conected();
	
 private:
	OneWire* _oneWire;
	DallasTemperature* _sensors;
	SoftwareSerial* _btSerial;
	uint8_t _Rx; 
	uint8_t _Tx;
	uint8_t _OneWPin;
	int _serialSpeed;
	int _index = 0;
	char command[BUFSIZE];
};


class BTSensorsManager
{
	protected:
	char * processRequest(char *  command);

	public:
	BTSensorsManager();
	BTSensorsManager(uint8_t Rx, uint8_t Tx);
	BTSensorsManager(uint8_t Rx, uint8_t Tx, int serialSpeed);
	int readDigital(uint8_t pin);
	int writeDigital(uint8_t pin, int val);
	int readAnalog(uint8_t pin);
	int writeAnalog(uint8_t pin, int val);
	float readTemp();
	void begin();
	bool conected();
char* getResponse();
	
	private:
	SoftwareSerial* _btSerial;
	uint8_t _Rx;
	uint8_t _Tx;
	int _serialSpeed;
	int _index = 0;
	char command[BUFSIZE];
};

#endif

