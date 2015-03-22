#include "BTSensorManager.h"

BTSensorManager::BTSensorManager()
{
	_serialSpeed = SERIALSPEED;
}

BTSensorManager::BTSensorManager(float serialSpeed)
{
	_serialSpeed = serialSpeed;
}

void BTSensorManager::begin()
{
	Serial2.begin(_serialSpeed);
	for (int i=0; i++; i<6) analogData[i]=0;
	for (int i=0; i++; i<13) digitalData[i]=0;
	temperature = 0;
}

void BTSensorManager::update()
{
	Serial2.begin(_serialSpeed);
	for (int i=0; i++; i<6) updateDigital(i);
	for (int i=0; i++; i<13) updateAnalog(i);
	updateTemp();
}

bool BTSensorManager::conected()
{
	return true;
}

void BTSensorManager::updateDigital(uint8_t pin)
{
	if (conected())
	{
		Serial2.print(RDD);
		Serial2.print(" ");
		Serial2.print(pin);
		Serial2.println();
	}
}

void BTSensorManager::setDigital(uint8_t pin, int val)
{
	if (conected())
	{
		Serial2.print(WRD);
		Serial2.print(" ");
		Serial2.print(pin);
		Serial2.print(" ");
		if (val > 0) {
			Serial2.print(HIGHVAL);
		}
		else {
			Serial2.print(LOWVAL);
		}
		Serial2.print(" ");
		Serial2.println();
		
	}
}

void BTSensorManager::updateAnalog(uint8_t pin)
{
	if (conected())
	{
		Serial2.print(RDA);
		Serial2.print(" ");
		Serial2.print(pin);
		Serial2.println();
		
	}
}

void BTSensorManager::setAnalog(uint8_t pin, int val)
{
	if (conected())
	{
		Serial2.print(WRA);
		Serial2.print(" ");
		Serial2.print(pin);
		Serial2.print(" ");
		Serial2.print(val);
		Serial2.println();
	}
}

void BTSensorManager::updateTemp()
{
	if (conected())
	{
		Serial2.print(TEM);
		Serial2.println();
	}
}

void BTSensorManager::writeLCD(char *  txt)
{
	if (conected())
	{
		Serial2.print(LCD);
		Serial2.println();
		Serial2.println(txt);
	}
}

bool BTSensorManager::getDigital(uint8_t pin)
{
	return digitalData[pin];
}

int BTSensorManager::getAnalog(uint8_t pin)
{
	return analogData[pin];
}

double BTSensorManager::getTemp()
{
	return temperature;
}

char * BTSensorManager::getTempText()
{
	return texTemperature;
}

void BTSensorManager::loop()
{
	if (conected()) {
		while ((Serial2.available()> 0) && (_index < BUFSIZE))  {
			char inByte = Serial2.read();
			if (inByte == 10 || inByte == 13 || inByte == 244){
				command[_index]= 0;
				processRequest(command);
				_index = 0;
			}
			else {
				command[_index]=inByte;
				_index++;
			}
		}
	}
}

void BTSensorManager::processRequest(char *  command)
{
	char *  code = command;
	command = strstr(command, " ");
	command[0]=0;
	command = &command[1];
	int icode = atoi(code);
	int value = 0;
	switch (icode){
		case BRDD:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			icode = atoi(code);
			value = atoi(command);
			digitalData[icode] =  value;
			break;
		case BWRD:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			icode = atoi(code);
			if (strstr(command, HIGHVAL) != 0) {
				digitalData[icode] =  HIGH;
			}
			if (strstr(command, LOWVAL) != 0) {
				digitalData[icode] =  LOW;
			}
			break;
		case BRDA:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			icode = atoi(code);
			value = atoi(command);
			analogData[icode] =  value;
			break;
		case BWRA:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			icode = atoi(code);
			value = atoi(command);
			analogData[icode] =  value;
			break;
		case BTEM:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			strcpy(texTemperature, code);
			temperature = atof(code);
			break;
		case BLCD:
			break;
		default:
			break;
	}
}
	
