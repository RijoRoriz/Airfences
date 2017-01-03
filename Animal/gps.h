#ifndef GPS_H__
#define GPS_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iomanip>


#include <wiringPi.h>
#include <wiringSerial.h>


#define PWR_GPS   1 //wiringPi 1 is BCM_GPIO 18 (12)

using namespace std;

class CGps
{
private:
	int gpsHandler;

	float mf_latitude;
	float mf_longitude;
	int mi_UTCtime;

	bool mb_gpsPower;
	bool mb_gpsData;

public:

	CGps();
	~CGps();

	unsigned int sendATCommand(const char *AT, const char *expected_answer, unsigned int timeout);

	void powerON();
	void powerOFF();
	void initGps();
	void readGps();
	bool gpsDataStatus();

	float getLatitude();
	float getLongitude();

};

#endif
