
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LCD.h>
#include "BTComLib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const uint8_t Rx = 10;
const uint8_t Tx = 11;
const uint8_t OneWPin = 1;

#define HUMIDITY_PIN 0
#define LIGHT_PIN 13
#define WIND_PIN 12

unsigned long previousMillis = 0;
const long interval = 5000;
unsigned long previousScreenMillis = 0;
const long screenInterval = 1000;
float temperature = 0;
BTRemoteSensors BTSensors(Rx, Tx, OneWPin, 9600);

void setup()
{
	lcd.begin(20,4);
	lcd.home ();                   // go home
	lcd.print("Loading data ");
	lcd.setCursor ( 0, 1 );        // go to the next line
	lcd.print ("Conecting to remote");
	lcd.setCursor ( 0, 2 );        // go to the next line
	lcd.print ("device");
	BTSensors.begin();
	pinMode(LIGHT_PIN, OUTPUT);
	pinMode(WIND_PIN, OUTPUT);
	temperature = BTSensors.getTemperature();
}

void loop()
{
	Serial.println("loop");
	unsigned long currentMillis = millis();
	if(currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		temperature = BTSensors.getTemperature();
	}
	BTSensors.loop();
	
	
	if(currentMillis - previousScreenMillis >= screenInterval) {
		previousScreenMillis = currentMillis;
		lcd.clear();
		lcd.home ();
		lcd.print("LUZ: ");
		if (BTSensors.getDigital(LIGHT_PIN)) lcd.print("ACTIVADA");
		else lcd.print("DESACTIVADA");
		lcd.setCursor ( 0, 1 );
		lcd.print("AIRE: ");
		if (BTSensors.getDigital(WIND_PIN)) lcd.print("ACTIVADO");
		else lcd.print("DESACTIVADO");
		lcd.setCursor ( 0, 2 );
		lcd.print ("TEMPERATURA: ");
		lcd.print (temperature);
		lcd.setCursor ( 0, 3 );
		lcd.print ("HUMEDAD: ");
		lcd.print ((1024-analogRead(HUMIDITY_PIN))/10.24);
	}
}