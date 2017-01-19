#include "leds.h"


CLeds :: CLeds()
{
	wiringPiSetup();

	pinMode(GREEN_ZONE, OUTPUT);
	pinMode(YELLOW_ZONE, OUTPUT);
	pinMode(RED_ZONE, OUTPUT);

	pinMode(PWR_STATUS , OUTPUT);
	digitalWrite(PWR_STATUS , HIGH);

	digitalWrite(GREEN_ZONE , LOW);
	digitalWrite(YELLOW_ZONE , LOW);
	digitalWrite(RED_ZONE , LOW);

}

CLeds :: ~CLeds()
{
}

void CLeds :: m_turnON_LedZone(int zone)
{
	switch (zone)
	{
		case OUTZONE:
		digitalWrite(GREEN_ZONE , HIGH);
		digitalWrite(YELLOW_ZONE , HIGH);
		digitalWrite(RED_ZONE , HIGH);
		break;

		case REDZONE:
		digitalWrite(RED_ZONE , HIGH);
		digitalWrite(GREEN_ZONE , LOW);
		digitalWrite(YELLOW_ZONE , LOW);
		break;

		case YELLOWZONE:
		digitalWrite(YELLOW_ZONE , HIGH);
		digitalWrite(GREEN_ZONE , LOW);
		digitalWrite(RED_ZONE , LOW);
		break;

		case GREENZONE:
		digitalWrite(GREEN_ZONE , HIGH);
		digitalWrite(YELLOW_ZONE, LOW);
		digitalWrite(RED_ZONE, LOW);
		break;

		default:
		break;
	}
}

void CLeds :: m_turnOFF()
{
	digitalWrite(GREEN_ZONE , LOW);
	digitalWrite(YELLOW_ZONE , LOW);
	digitalWrite(RED_ZONE , LOW);
}

void CLeds :: m_batteryWarning()
{
	// digitalWrite(PWR_STATUS , HIGH);
	// delay(1000);
	// digitalWrite(PWR_STATUS , LOW);
	// delay(1000);
}
