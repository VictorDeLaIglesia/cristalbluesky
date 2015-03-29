#include <DS1307.h>
#include <Wire.h>
#include "BTSensorManager.h"
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <UIPEthernet.h>


uint8_t shour = 6;
uint8_t sminute = 0;
uint8_t ehour = 22;
uint8_t eminute = 0;

// ThingSpeak Settings
EthernetClient cliente;
byte ThingSpeakServer[]  = { 184, 106, 153, 149 }; // IP Address for the ThingSpeak API
String writeAPIKey = "0H0FP0SAXTO96G0L";    // Write API Key for a ThingSpeak Channel
const int updateInterval = 60000;        // Time interval in milliseconds to update ThingSpeak
unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
int resetCounter = 0;

DS1307 dsclock;//define a object of DS1307 class
BTSensorManager sensorManager(9600);
unsigned long previousMillis = 0;
const long interval = 3000;
uint8_t SS_SDReader = 4;
byte mac[] = {0x90,0xA2,0xDA,0x00,0x26,0xEB};
byte ip[] = {192,168,1,55};
byte dnsip[] = {192,168,1,1};
byte gateway[] = {192,168,1,1};
byte subnet[]  = {255,255,255,0};
char rootFileName[] = "index.htx";  //root file name is what the homepage will be.
EthernetServer server(9090);

Sd2Card card;  //SD Stuff
SdVolume volume;
SdFile root;
SdFile file;
SdFile file2;

uint8_t lightState;
uint8_t windState;
bool automatic;


#define HUMIDITY_PIN 0
#define LIGHT_PIN 13
#define WIND_PIN 12

#define BUFSIZ 100

//#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.print (x)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#define DEBUG_PRINT2(x, y)  Serial.print (x, y)
#define DEBUG_PRINTLN2(x, y)  Serial.println (x, y)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT2(x, y)
#define DEBUG_PRINTLN2(x, y) 
#endif

#define error(s) error_P(PSTR(s))  //SD card errors stored in Program memory
void error_P(const char* str) {  //Error function
	DEBUG_PRINT("error: ");
	DEBUG_PRINTLN(str);
	if (card.errorCode()) {
		DEBUG_PRINT("SD error: ");
		DEBUG_PRINT2(card.errorCode(), HEX);
		DEBUG_PRINT(',');
		DEBUG_PRINTLN2(card.errorData(), HEX);
	}
}

void setup()
{
	Serial.begin(115200);
	dsclock.begin();
	sensorManager.begin();
	DEBUG_PRINT("Free RAM: ");
	DEBUG_PRINTLN(FreeRam());
	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);
	if (!card.init(SPI_DIV3_SPEED, SS_SDReader)) error("card.init failed!");  //If you are having errors when reading from the SD card, change FULL to HALF
	if (!volume.init(&card)) error("vol.init failed!");
	DEBUG_PRINT("Volume is FAT");
	DEBUG_PRINTLN2(volume.fatType(), DEC);
	DEBUG_PRINTLN();
	if (!root.openRoot(&volume)) error("openRoot failed");
	DEBUG_PRINTLN("Files found in root:");
	root.ls(LS_DATE | LS_SIZE);
	DEBUG_PRINTLN();
	DEBUG_PRINTLN("Files found in all dirs:");
	#ifdef DEBUG
	root.ls(LS_R);
	#endif
	DEBUG_PRINTLN();
	DEBUG_PRINTLN("Done");
	Ethernet.begin(mac, ip, dnsip, gateway, subnet);
	server.begin();
	lightState = 0;
	windState = 0;
	automatic = true;
	sensorManager.setDigital(LIGHT_PIN, lightState);
	sensorManager.setDigital(WIND_PIN, windState);
	shour = getHour("shour", 1);
	ehour = getHour("ehour", 6);
	sminute = getHour("sminute", 0);
	eminute = getHour("eminute", 0);
}

int getHour(char* filestring, int defval) {
	long val = defval;
	if (! file2.open(&root, filestring, O_READ)) {
		file2.open(&root, filestring, O_CREAT | O_APPEND | O_WRITE);
		file2.print(val);
	}
	else {  
		val = 0;
		int c = file2.read();
		// ignore non numeric leading characters
		if(c < 0) {
			val =  0; // zero returned if timeout
		}
		else {
			do{
				if(c >= '0' && c <= '9')        // is c a digit?
				val = val * 10 + c - '0';
				c = file2.read();
			}
			while( (c >= '0' && c <= '9') );
		}
	}
	file2.close();
	DEBUG_PRINT(filestring);
	DEBUG_PRINT(" ");
	DEBUG_PRINT((int)val);
	DEBUG_PRINTLN();
	return (int)val;
}

void setHour(char* filestring, int defval) {
	if (! file2.open(&root, filestring, O_WRITE)) {
		file2.open(&root, filestring, O_CREAT | O_APPEND | O_WRITE);
		file2.print(defval);
	}
	else {
		file2.println(defval);
	}
	file2.close();
}

char* getData(char * datafield)
{
	char *data = (char *) malloc(sizeof(char));
	strcpy(data, "");
	if (strstr(datafield, "temp") != 0)
		strcat(data, sensorManager.getTempText());
	else if (strstr(datafield, "humidity") != 0){
		int humi = (1024-sensorManager.getAnalog(HUMIDITY_PIN))/10.24;
		String dataHumi = String();
		dataHumi.concat(humi);
		strcat(data, dataHumi.c_str());
	}
	else if (strstr(datafield, "light") != 0){
		if (lightState == 1) strcat(data, "on");
		else strcat(data, "off");
		if (!automatic) strcat(data, "m");
	}
	else if (strstr(datafield, "wind") != 0){
		if (windState == 1) strcat(data, "on");
		else strcat(data, "off");
	}
	else if (strstr(datafield, "clock") != 0) {
		dsclock.getTime();
		String clocktime = String();
		clocktime.concat(dsclock.hour);
		clocktime.concat(":");
		clocktime.concat(dsclock.minute);
		clocktime.concat(":");
		clocktime.concat(dsclock.second);
		strcat(data, clocktime.c_str());
	}
	else if (strstr(datafield, "starthour") != 0) {
		String string = String();
		if (shour < 10)  string.concat("0");
		string.concat(shour);
		strcat(data, string.c_str());
	}
	else if (strstr(datafield, "startminute") != 0) {
		String string = String();
		if (sminute < 10)  string.concat("0");
		string.concat(sminute);
		strcat(data, string.c_str());
	}
	else if (strstr(datafield, "endhour") != 0) {
		String string = String();
		if (ehour < 10)  string.concat("0");
		string.concat(ehour);
		strcat(data, string.c_str());
	}
	else if (strstr(datafield, "endminute") != 0) {
		String string = String();
		if (eminute < 10)  string.concat("0");
		string.concat(eminute);
		strcat(data, string.c_str());
	}
	else strcat(data, "");
	char c2[1];
	c2[0] = '\0';
	strcat(data,  c2);
	return data;
}

bool greaterTime(uint8_t thour, uint8_t tminute, uint8_t hour, uint8_t minute)
{
	if (thour < hour) return true;
	else if (thour == hour) {
		if (tminute < minute) return true;
		else return  false;
	}
	else return false;
}

bool smallerTime(uint8_t thour, uint8_t tminute, uint8_t hour, uint8_t minute)
{
	if (thour > hour) return true;
	else if (thour == hour) {
		if (tminute > minute) return true;
		else return  false;
	}
	else return false;
}

void processCommand(char * command)
{
	DEBUG_PRINTLN(command);
	char *tempComand;
	char *params;
	if (strstr(command, "=") != 0) {
		tempComand = strstr(command, "=");
		tempComand[0] = '\0';
		params = &tempComand[1];
		if (strstr(command, "turnon")){
			automatic = true;
			params[2] = '\0';
			shour = atoi(params);
			setHour("shour", shour);
			params = &params[5];
			params[7] = '\0';
			sminute = atoi(params);
			setHour("sminute", sminute);
		}
		else if (strstr(command, "turnoff"))
		{
			automatic = true;
			params[2] = '\0';
			ehour = atoi(params);
			setHour("ehour", ehour);
			params = &params[5];
			params[7] = '\0';
			eminute = atoi(params);
			setHour("eminute", eminute);
		}
		else if (strstr(command, "fan"))
		{
			if (windState == 1) windState = 0;
			else windState = 1;
			sensorManager.setDigital(WIND_PIN, windState);
		}
		else if (strstr(command, "manuallight"))
		{
			automatic = false;
			if (lightState > 0) lightState = 0;
			else lightState = 1;
			sensorManager.setDigital(LIGHT_PIN, lightState);
		}
		else if (strstr(command, "hour"))
		{
			params[2] = '\0';
			uint8_t chour = atoi(params);
			params = &params[5];
			params[7] = '\0';
			uint8_t cminute = atoi(params);
			dsclock.fillByHMS(chour, cminute, 0);
			dsclock.setTime();
		}
		else if (strstr(command, "date"))
		{
			params[4] = '\0';
			uint8_t cyear = atoi(params);
			params = &params[5];
			params[2] = '\0';
			uint8_t cmonth = atoi(params);
			params = &params[3];
			params[2] = '\0';
			uint8_t cday = atoi(params);
			dsclock.fillByYMD(cyear, cmonth, cday);
			dsclock.setTime();
		}
	}
}

void processGetCommand(char * command)
{
	DEBUG_PRINTLN(command);
	char *tempComand;
	while (strstr(command, "&") != 0) {
		tempComand = strstr(command, "&");
		tempComand[0] = '\0';
		processCommand(command);
		strcpy(command, &tempComand[1]);
	}
	processCommand(command);
}

void loop()
{
		sensorManager.loop();
		unsigned long currentMillis = millis();
		if(currentMillis - previousMillis >= interval) {
			previousMillis = currentMillis;
			lightState = sensorManager.getDigital(LIGHT_PIN);
			windState = sensorManager.getDigital(WIND_PIN);
			sensorManager.updateTemp();
			sensorManager.updateAnalog(HUMIDITY_PIN);
			dsclock.getTime();
			if (automatic) {
				if (greaterTime(shour, sminute, dsclock.hour, dsclock.minute) && smallerTime(ehour, eminute, dsclock.hour, dsclock.minute)) {
					lightState = 1;	
					sensorManager.setDigital(LIGHT_PIN, lightState);
				}
				else {
					lightState = 0;
					sensorManager.setDigital(LIGHT_PIN, lightState);
				}
			}
		}
		char clientline[BUFSIZ];
		char *command;
		char *filename;
		char datafield[20];
		int index = 0;
		bool process = false;
		bool processing = false;
		EthernetClient client = server.available();
		if (client) {
			boolean current_line_is_blank = true;
			index = 0;
			while (client.connected()) {
				if (client.available()) {
					char c = client.read();
					if (c != '\n' && c != '\r') {
						clientline[index] = c;
						index++;
						if (index >= BUFSIZ)
						index = BUFSIZ -1;
						continue;
					}
					clientline[index] = 0;
					filename = 0;
					DEBUG_PRINTLN(clientline);
					if (strstr(clientline, "GET / ") != 0) {  //If you are going to the homepage, the filename is set to the rootFileName
						filename = rootFileName;
					}
					if (strstr(clientline, "GET /") != 0) {
						if (!filename) filename = clientline + 5;  //gets rid of the GET / in the filename
						if (strstr(filename, "?") != 0)  {
							command = (strstr(clientline, "?"))+1;
							(strstr(clientline, "?"))[0] = 0;
							(strstr(command, " HTTP"))[0] = 0; //gets rid of everything from HTTP to the end.
							processGetCommand(command);
							} else {
							(strstr(clientline, " HTTP"))[0] = 0;  //gets rid of everything from HTTP to the end.
						}
						DEBUG_PRINTLN(filename);
						if (! file.open(&root, filename, O_READ)) {  //if the file doesn't exist a 404 is sent
							client.println("HTTP/1.1 404 Not Found");
							client.println("Content-Type: text/html");
							client.println();
							client.println("<h2>File Not Found. Better luck next time.!</h2>");
							break;
						}
						DEBUG_PRINTLN("Opened!");
						client.println("HTTP/1.1 200 OK");
						if (strstr(filename, ".htm") != 0)  //Sets content type.
						client.println("Content-Type: text/html");
						else if (strstr(filename, ".htx") != 0){
							process = true;
							client.println("Content-Type: text/html");
						}
						else if (strstr(filename, ".css") != 0)
						client.println("Content-Type: text/css");
						else if (strstr(filename, ".png") != 0)
						client.println("Content-Type: image/png");
						else if (strstr(filename, ".jpg") != 0)
						client.println("Content-Type: image/jpeg");
						else if (strstr(filename, ".gif") != 0)
						client.println("Content-Type: image/gif");
						else if (strstr(filename, ".3gp") != 0)
						client.println("Content-Type: video/mpeg");
						else if (strstr(filename, ".pdf") != 0)
						client.println("Content-Type: application/pdf");
						else if (strstr(filename, ".js") != 0)
						client.println("Content-Type: application/x-javascript");
						else if (strstr(filename, ".xml") != 0)
						client.println("Content-Type: application/xml");
						else
						client.println("Content-Type: text");
						client.println();
						int16_t c;
						processing = false;
						while ((c = file.read()) >= 0) {
							if ((process) && ((char)c == '@') && (!processing))  {
								processing = true;
								strcpy(datafield,  "");
								}else if ((process) && (processing)) {
								if ((char)c == '@') {
									char *ret;
									ret = getData(datafield);
									client.write(ret);
									DEBUG_PRINT(ret);
									processing = false;
								}
								else {
									char c2[2];
									c2[0] = c;
									c2[1] = '\0';
									strcat(datafield,  c2);
								}
								}else {
								client.write((char)c);
							}
						}
						file.close();
						process=false;
					}
					else {
						client.println("HTTP/1.1 404 Not Found");
						client.println("Content-Type: text/html");
						client.println();
						client.println("<h2>File Not Found. Better luck next time.!!</h2>");
					}
					break;
				}
			}
			client.flush();
		}
		file.close();
		server.begin();
		
		// Update ThingSpeak
		if (cliente.available())
		{
			char c = cliente.read();
			DEBUG_PRINTLN(c);
		}
		  
		if (!cliente.connected() && lastConnected) {
			cliente.stop();
		}
		if (((millis() - lastConnectionTime > updateInterval) || (lastConnectionTime == 0)) && (!cliente.connected()))
		{
			float humidity = (1024-sensorManager.getAnalog(HUMIDITY_PIN))/10.24;
			updateThingSpeak("field1="+String(sensorManager.getTemp(), DEC)+"&field2="+String(humidity, DEC)+"&field3="+String(lightState, DEC)+"&field4="+String(windState, DEC));
		}
		lastConnected = cliente.connected();
}

void updateThingSpeak(String tsData)
{
	if (cliente.connect(ThingSpeakServer, 80))
	{
		DEBUG_PRINTLN("Connected to ThingSpeak...");
		DEBUG_PRINTLN(FreeRam());
		if (!cliente.connected()) {;}
		cliente.print("POST /update HTTP/1.1\n");
		cliente.print("Host: api.thingspeak.com\n");
		cliente.print("Connection: close\n");
		cliente.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
		cliente.print("Content-Type: application/x-www-form-urlencoded\n");
		cliente.print("Content-Length: ");
		cliente.print(tsData.length());
		DEBUG_PRINTLN(tsData);
		cliente.print("\n\n");
		cliente.print(tsData);
		lastConnectionTime = millis();
		resetCounter = 0;
	}
	else
	{
		DEBUG_PRINTLN("Connection Failed.");
		DEBUG_PRINTLN(FreeRam());
		DEBUG_PRINTLN();
		resetCounter++;
		if (resetCounter >=10 ) {resetEthernetShield();}
		lastConnectionTime = millis();
	}
}

void resetEthernetShield()
{
	DEBUG_PRINTLN("Resetting Ethernet Shield.");
	cliente.stop();
	delay(1000);
	
	Ethernet.begin(mac, ip, dnsip, gateway, subnet);
	delay(1000);
	
	server.begin();
	delay(1000);
	
	lastConnected = false;
}