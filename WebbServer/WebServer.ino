//This is the basic SDFat Webserver sketch, without any modifications
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <UIPEthernet.h>

uint8_t SS_SDReader = 4;
byte mac[] = {
0x90,0xA2,0xDA,0x00,0x26,0xEB};
byte ip[] = {
192,168,1,55};
char rootFileName[] = "index.htx";  //root file name is what the homepage will be.
EthernetServer server(8080);

Sd2Card card;  //SD Stuff
SdVolume volume;
SdFile root;
SdFile file;

#define error(s) error_P(PSTR(s))  //SD card errors stored in Program memory
void error_P(const char* str) {  //Error function
	PgmPrint("error: ");
	SerialPrintln_P(str);
	if (card.errorCode()) {
		PgmPrint("SD error: ");
		Serial.print(card.errorCode(), HEX);
		Serial.print(',');
		Serial.println(card.errorData(), HEX);
	}
	while(1);
}

void setup() {  //setup stuff
	Serial.begin(115200);
	PgmPrint("Free RAM: ");
	Serial.println(FreeRam());
	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);
	if (!card.init(SPI_DIV3_SPEED, SS_SDReader)) error("card.init failed!");  //If you are having errors when reading from the SD card, change FULL to HALF
	if (!volume.init(&card)) error("vol.init failed!");
	PgmPrint("Volume is FAT");
	Serial.println(volume.fatType(),DEC);
	Serial.println();
	if (!root.openRoot(&volume)) error("openRoot failed");
	PgmPrintln("Files found in root:");
	root.ls(LS_DATE | LS_SIZE);
	Serial.println();
	PgmPrintln("Files found in all dirs:");
	root.ls(LS_R);
	Serial.println();
	PgmPrintln("Done");
	Ethernet.begin(mac, ip);
	server.begin();
}
#define BUFSIZ 100  

char* getData(char * datafield)
{
	char *data = (char *) malloc(sizeof(char));
	strcpy(data, "");
	if (strstr(datafield, "temp") != 0)
		return "23,67";
	else if (strstr(datafield, "humidity") != 0)
		strcat(data, "96");
	else if (strstr(datafield, "light") != 0)
		strcat(data, "on");
	else if (strstr(datafield, "wind") != 0)
		strcat(data, "off");
	else if (strstr(datafield, "hour") != 0)
		strcat(data, "23:67");
	else if (strstr(datafield, "starthour") != 0)
		strcat(data, "06:00");
	else if (strstr(datafield, "endhour") != 0)
		strcat(data, "22:00");
	else strcat(data, "");
	char c2[1];
	c2[0] = '\0';
	strcat(data,  c2);
	
	Serial.print(data);
	return data;
}

void loop()
{
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
				Serial.println(clientline);
				if (strstr(clientline, "GET / ") != 0) {  //If you are going to the homepage, the filename is set to the rootFileName
					filename = rootFileName;
				}
				if (strstr(clientline, "GET /") != 0) {
					if (!filename) filename = clientline + 5;  //gets rid of the GET / in the filename
					if (strstr(filename, "?") != 0)  {
						command = (strstr(clientline, "?"))+1;
						(strstr(clientline, "?"))[0] = 0;
						(strstr(command, " HTTP"))[0] = 0; //gets rid of everything from HTTP to the end.
						Serial.println(command);
					} else {
						(strstr(clientline, " HTTP"))[0] = 0;  //gets rid of everything from HTTP to the end.
					}
					Serial.println(filename);
					if (! file.open(&root, filename, O_READ)) {  //if the file doesn't exist a 404 is sent
						client.println("HTTP/1.1 404 Not Found");
						client.println("Content-Type: text/html");
						client.println();
						client.println("<h2>File Not Found. Better luck next time.!</h2>");
						break;
					}
					Serial.println("Opened!");
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
								Serial.print(ret);
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
							Serial.write((char)c);
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
		delay(100);
		client.stop();
		FreeRam();
		file.close();
	}
}