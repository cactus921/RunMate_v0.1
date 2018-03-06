/*
 Name:		RunMate1.ino
 Created:	3/6/2018 9:28:18 AM
 Author:	Eran
*/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

static const int GPS_RXPin = 4, GPS_TXPin = 3;
static const int BT_RXPin = 10, BT_TXPin = 11;
static const uint32_t GPSBaud = 9600;
static const uint32_t BTBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial GPSSerial(GPS_RXPin, GPS_TXPin);

// The serial connection to the BT device
//SoftwareSerial BTSerial(BT_RXPin, BT_TXPin);
//int BTData = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	GPSSerial.begin(GPSBaud);
	//BTSerial.begin(BTBaud);

	Serial.println(F("RunMate.ino"));
	Serial.println(F("First test of RunMate GPS Tracker"));
	Serial.println(F("Based on TinyGPS++ by Mikal Hart"));
	Serial.println(F("by Eran Kleiner"));
	Serial.println();
	Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date  Alt  Chars  Sentences  Checksum"));
	Serial.println(F("           (deg)      (deg)       Age                      Age   (m)   RX      RX        Fail"));
	Serial.println(F("-------------------------------------------------------------------------------------------------"));
}

// the loop function runs over and over again until power down or reset
void loop() {
	printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
	printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
	printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
	printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
	printInt(gps.location.age(), gps.location.isValid(), 5);
	printDateTime(gps.date, gps.time);
	printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);

	printInt(gps.charsProcessed(), true, 6);
	printInt(gps.sentencesWithFix(), true, 10);
	printInt(gps.failedChecksum(), true, 9);
	Serial.println();

	//if (BTSerial.available()) {
	//	BTData = BTSerial.read();
	//	Serial.write(BTData);
	//	Serial.println(BTData);
	//	BTSerial.write(BTData);
	//}

	smartDelay(1000);

	if (millis() > 5000 && gps.charsProcessed() < 10)
		Serial.println(F("No GPS data received: check wiring"));
}


// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
	unsigned long start = millis();
	do
	{
		while (GPSSerial.available())
			gps.encode(GPSSerial.read());
	} while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
	if (!valid)
	{
		while (len-- > 1)
			Serial.print('*');
		Serial.print(' ');
	}
	else
	{
		Serial.print(val, prec);
		int vi = abs((int)val);
		int flen = prec + (val < 0.0 ? 2 : 1); // . and -
		flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
		for (int i = flen; i<len; ++i)
			Serial.print(' ');
	}
	smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
	char sz[32] = "*****************";
	if (valid)
		sprintf(sz, "%ld", val);
	sz[len] = 0;
	for (int i = strlen(sz); i<len; ++i)
		sz[i] = ' ';
	if (len > 0)
		sz[len - 1] = ' ';
	Serial.print(sz);
	smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
	if (!d.isValid())
	{
		Serial.print(F("********** "));
	}
	else
	{
		char sz[32];
		sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
		Serial.print(sz);
	}

	if (!t.isValid())
	{
		Serial.print(F("******** "));
	}
	else
	{
		char sz[32];
		sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
		Serial.print(sz);
	}

	printInt(d.age(), d.isValid(), 5);
	smartDelay(0);
}

static void printStr(const char *str, int len)
{
	int slen = strlen(str);
	for (int i = 0; i<len; ++i)
		Serial.print(i<slen ? str[i] : ' ');
	smartDelay(0);
}