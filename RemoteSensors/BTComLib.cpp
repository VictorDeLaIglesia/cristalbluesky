#include "BTComLib.h"

BTRemoteSensors::BTRemoteSensors()
{
	_Rx = RX;
	_Tx = TX;
	_OneWPin = OWP;
	_serialSpeed = SERIALSPEED;
}

BTRemoteSensors::BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin)
{
	_Rx = Rx;
	_Tx = Tx;
	_OneWPin = OneWPin;
	_serialSpeed = SERIALSPEED;
}

BTRemoteSensors::BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin, int serialSpeed)
{
	_Rx = Rx;
	_Tx = Tx;
	_OneWPin = OneWPin;
	_serialSpeed = serialSpeed;
}

void BTRemoteSensors::begin()
{
	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	_oneWire = new OneWire(_OneWPin);
	// Pass our oneWire reference to Dallas Temperature.
	_sensors = new DallasTemperature(_oneWire);
	_sensors->begin();
	this->_btSerial = new SoftwareSerial(_Rx, _Tx);
	while (!conected()) 
	{
		// bucle until bt is conected
		delay(200);
	} 
	if (conected()) {
		_btSerial->begin(_serialSpeed);
	} 
}

char * BTRemoteSensors::processRequest(char *  command)
{
	char *  code = command;
	command = strstr(command, " ");
	command[0]=0;
	command = &command[1];
	int icode = atoi(code);
	int value = 0;
	switch (icode){
		case RDD:
		icode = atoi(command);
		_btSerial->print(BRDD);
		_btSerial->print(" ");
		_btSerial->print(icode);
		_btSerial->print(" ");
		_btSerial->print(digitalRead(icode));
		_btSerial->println();
		break;
		case WRD:
		code = command;
		command = strstr(command, " ");
		command[0]=0;
		command = &command[1];
		icode = atoi(code);
		_btSerial->print(BWRD);
		_btSerial->print(" ");
		_btSerial->print(icode);
		_btSerial->print(" ");
		value = atoi(command);
		_btSerial->print(value);
		_btSerial->print(" ");
		pinMode(icode, OUTPUT);
		digitalWrite(icode, value);
		_btSerial->print(digitalRead(icode));
		_btSerial->println();
		break;
		case RDA:
		icode = atoi(command);
		_btSerial->print(BRDA);
		_btSerial->print(" ");
		_btSerial->print(icode);
		_btSerial->print(" ");
		_btSerial->print(analogRead(icode));
		_btSerial->println();
		break;
		case WRA:
		code = command;
		command = strstr(command, " ");
		command[0]=0;
		command = &command[1];
		icode = atoi(code);
		_btSerial->print(BWRA);
		_btSerial->print(" ");
		_btSerial->print(icode);
		_btSerial->print(" ");
		value = atoi(command);
		_btSerial->print(value);
		_btSerial->print(" ");
		analogWrite(icode, value);
		_btSerial->print(analogRead(icode));
		_btSerial->println();
		break;
		case TEM:
		_btSerial->print(BTEM);
		_btSerial->print(" ");
		_sensors->requestTemperatures();
		_btSerial->print(_sensors->getTempCByIndex(0));
		_btSerial->println();
		uint8_t* deviceAddress2;
		const uint8_t deviceAddress[8] = {0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA};
		_sensors->getAddress(deviceAddress2, 0);
		for (int i=0; i<8 ; i++) {
			_btSerial->print(deviceAddress2[i]);
		}
		break;
		case LCD:
		_btSerial->print(BLCD);
		_btSerial->print(" ");
		_btSerial->println();
		break;
		default:
		_btSerial->print(BERR);
		_btSerial->println();
		
	}
	return command;
}

void BTRemoteSensors::loop()
{
	if (conected()) {
		while ((_btSerial->available()> 0) && (_index < BUFSIZE))  {
			char inByte = _btSerial->read();
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

bool BTRemoteSensors::conected()
{
	return TRUE;
}

// Sensors manager class

BTSensorsManager::BTSensorsManager()
{
	_Rx = RX;
	_Tx = TX;
	_serialSpeed = SERIALSPEED;
}

BTSensorsManager::BTSensorsManager(uint8_t Rx, uint8_t Tx)
{
	_Rx = Rx;
	_Tx = Tx;
	_serialSpeed = SERIALSPEED;
}

BTSensorsManager::BTSensorsManager(uint8_t Rx, uint8_t Tx, int serialSpeed)
{
	_Rx = Rx;
	_Tx = Tx;
	_serialSpeed = serialSpeed;
}

void BTSensorsManager::begin()
{
	this->_btSerial = new SoftwareSerial(_Rx, _Tx);
	_btSerial->begin(_serialSpeed);
}

bool BTSensorsManager::conected()
{
	return TRUE;
}

int BTSensorsManager::readDigital(uint8_t pin)
{
	if (conected()) 
	{
		_btSerial->print(RDD);
		_btSerial->print(" ");
		_btSerial->print(pin);
		_btSerial->println();
		char* command = getResponse();
	}
}

int BTSensorsManager::writeDigital(uint8_t pin, int val)
{
	if (conected())
	{
		_btSerial->print(WRD);
		_btSerial->print(" ");
		_btSerial->print(pin);
		_btSerial->print(" ");
		_btSerial->print(val);
		_btSerial->println();
		char* command = getResponse();
		
	}
}

int BTSensorsManager::readAnalog(uint8_t pin)
{
	if (conected())
	{
		_btSerial->print(RDA);
		_btSerial->print(" ");
		_btSerial->print(pin);
		_btSerial->println();
		char* command = getResponse();
		
	}
}

int BTSensorsManager::writeAnalog(uint8_t pin, int val)
{
	if (conected())
	{
		_btSerial->print(WRA);
		_btSerial->print(" ");
		_btSerial->print(pin);
		_btSerial->print(" ");
		_btSerial->print(val);
		_btSerial->println();
		char* command = getResponse();
	}
}

float BTSensorsManager::readTemp()
{
	if (conected())
	{
		_btSerial->print(TEM);
		_btSerial->println();
		char* command = getResponse();
	}
}

char* BTSensorsManager::getResponse()
{
	return "hol";
}
