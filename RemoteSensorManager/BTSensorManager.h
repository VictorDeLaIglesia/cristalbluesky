// BTSensorManager.h

#ifndef _BTSENSORMANAGER_h
#define _BTSENSORMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

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

#define SERIALSPEED 9600

int const BUFSIZE = 40;

class BTSensorManager
{
		protected:

		public:
			BTSensorManager();
			BTSensorManager(float serialSpeed);
			void begin();
			void loop();
			void updateAnalog(uint8_t pin);
			void updateDigital(uint8_t pin);
			void updateTemp();
			void setDigital(uint8_t pin, int val);
			void setAnalog(uint8_t pin, int val);
			bool getDigital(uint8_t pin);
			int getAnalog(uint8_t pin);
			double getTemp();
			char* getTempText();
			void writeLCD(char *  txt);
			bool conected();

		private:
			void processRequest(char *  command);
			void update();
			float _serialSpeed;
			int _index = 0;
			char command[BUFSIZE];
			bool digitalData[13];
			int analogData[6];
			double temperature;
			char texTemperature[10];
};

#endif

