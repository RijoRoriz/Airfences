#include "leds.h"


CLeds :: CLeds()
{
	wiringPiSetup();

	pinMode(LED_GREEN_ZONE, OUTPUT);
	pinMode(LED_YELLOW_ZONE, OUTPUT);
	pinMode(LED_RED_ZONE, OUTPUT);

	pinMode(LED_PWR_STATUS , OUTPUT);
	digitalWrite(LED_PWR_STATUS , HIGH);

	digitalWrite(LED_GREEN_ZONE , LOW);
	digitalWrite(LED_YELLOW_ZONE , LOW);
	digitalWrite(LED_RED_ZONE , LOW);

}

CLeds :: ~CLeds()
{
}

void CLeds :: m_turnON_LedZone(int zone)
{
	switch (zone)
	{
		case OUTZONE:
		digitalWrite(LED_GREEN_ZONE , HIGH);
		digitalWrite(LED_YELLOW_ZONE , HIGH);
		digitalWrite(LED_RED_ZONE , HIGH);
		break;

		case REDZONE:
		digitalWrite(LED_RED_ZONE , HIGH);
		digitalWrite(LED_GREEN_ZONE , LOW);
		digitalWrite(LED_YELLOW_ZONE , LOW);
		break;

		case YELLOWZONE:
		digitalWrite(LED_YELLOW_ZONE , HIGH);
		digitalWrite(LED_GREEN_ZONE , LOW);
		digitalWrite(LED_RED_ZONE , LOW);
		break;

		case GREENZONE:
		digitalWrite(LED_GREEN_ZONE , HIGH);
		digitalWrite(LED_YELLOW_ZONE, LOW);
		digitalWrite(LED_RED_ZONE, LOW);
		break;

		default:
		break;
	}
}

void CLeds :: m_turnOFF()
{
	digitalWrite(LED_GREEN_ZONE , LOW);
	digitalWrite(LED_YELLOW_ZONE , LOW);
	digitalWrite(LED_RED_ZONE , LOW);
}

void CLeds :: m_BatteryLedStatus(bool state)
{
	if(state) {
		digitalWrite(LED_PWR_STATUS , HIGH);
	}
	else {
		digitalWrite(LED_PWR_STATUS , LOW);
	}
}

void CLeds :: m_batteryWarning()
{
	// digitalWrite(LED_PWR_STATUS , HIGH);
	// delay(1000);
	// digitalWrite(LED_PWR_STATUS , LOW);
	// delay(1000);
}
