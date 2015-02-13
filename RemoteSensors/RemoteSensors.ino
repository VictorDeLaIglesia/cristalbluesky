#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 3
#define RDD 0

int const BUFSIZE = 40;

//const int RDD = 0;
const int WRD = 1;
const int RDA = 2;
const int WRA = 3;
const int TEM = 4;
const int LCD = 5;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

char command[BUFSIZE];

void setup()
{
	Serial.begin(9600);
	Serial.write("Conectado\n");
	sensors.begin();
}

char * processCommand(char *  command)
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
			Serial.print("BRDD ");
			Serial.print(icode);
			Serial.print(" ");
			Serial.print(digitalRead(icode));
			Serial.println();
		break;
		case WRD:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			icode = atoi(code);
			Serial.print("BWRD ");
			Serial.print(icode);
			Serial.print(" ");
			value = atoi(command);
			Serial.print(value);
			Serial.print(" ");
			digitalWrite(icode, value);
			Serial.print(digitalRead(icode));
			Serial.println();
		break;
		case RDA:
			icode = atoi(command);
			Serial.print("BRDA ");
			Serial.print(icode);
			Serial.print(" ");
			Serial.print(analogRead(icode));
			Serial.println();
		break;
		case WRA:
			code = command;
			command = strstr(command, " ");
			command[0]=0;
			command = &command[1];
			icode = atoi(code);
			Serial.print("BWRA ");
			Serial.print(icode);
			Serial.print(" ");
			value = atoi(command);
			Serial.print(value);
			Serial.print(" ");
			analogWrite(icode, value);
			Serial.print(analogRead(icode));
			Serial.println();
		break;
		case TEM:
			Serial.print("BTEM ");
			sensors.requestTemperatures();
			Serial.print(sensors.getTempCByIndex(0));
			Serial.print(" ");
			Serial.print(sensors.getDeviceCount());
			Serial.println();
		break;
		case LCD:
			Serial.print("BLCD ");
		break;
	}
	return command;
}

void loop()
{
	int index = 0;
	while (Serial.available() > 0) {
		index = 0;
		while ((Serial.available()> 0) && (index < BUFSIZE))  {
			char inByte = Serial.read();
			if (inByte == 10 || inByte == 13 || inByte == 244){
				command[index]= 0; 
				if (strlen(command) >= 1)
					processCommand(command);
				index = 0;
			}
			else {
				command[index]=inByte;
				index++;
			}
		}
	}
}