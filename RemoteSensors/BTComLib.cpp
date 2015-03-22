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

BTRemoteSensors::BTRemoteSensors(uint8_t Rx, uint8_t Tx, uint8_t OneWPin, long serialSpeed)
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
    this->_btSerial->begin(_serialSpeed);
	processRequest("2 13 1");
	processRequest("2 12 1");
}

char * BTRemoteSensors::processRequest(char *  command)
{
	char *  code = command;
	command = strstr(command, " ");
	command[0]=0;
	command = &command[1];
	int icode = atoi(code);
	int value = 0;
	double fake = 0 ;
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
			if (strstr(command, HIGHVAL) != 0) {
				_btSerial->print(BWRD);
				_btSerial->print(" ");
				_btSerial->print(icode);
				_btSerial->print(" ");
				_btSerial->print(HIGHVAL);
				_btSerial->print(" ");
				if (digitalData[icode] != HIGH) {
					digitalData[icode] = HIGH;
					digitalWrite(icode, HIGH);
				}
			}
			if (strstr(command, LOWVAL) != 0) {
				_btSerial->print(BWRD);
				_btSerial->print(" ");
				_btSerial->print(icode);
				_btSerial->print(" ");
				_btSerial->print(LOW);
				_btSerial->print(" ");
				if (digitalData[icode] != LOW) {
					digitalData[icode] = LOW;
					digitalWrite(icode, LOW);
				}
			}
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
			_btSerial->print(value);
			_btSerial->println();
			break;
		case TEM:
			_btSerial->print(BTEM);
			_btSerial->print(" ");
			_sensors->requestTemperatures();
			_btSerial->print(_sensors->getTempCByIndex(0));
			_btSerial->println();
			break;
		case LCD:
			_btSerial->print(BLCD);
			_btSerial->print(" ");
			_btSerial->println();
			break;
		default:
			_btSerial->print(BERR);
			_btSerial->println();
			break;
	}
	return command;
}

float BTRemoteSensors::getTemperature() {
	_sensors->requestTemperatures();
	return _sensors->getTempCByIndex(0);
}

bool BTRemoteSensors::getDigital(int pin) {
	return digitalData[pin];
}

int BTRemoteSensors::getAnalog(int pin) {
	return analogData[pin];
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

