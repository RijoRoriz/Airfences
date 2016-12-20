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

	mb_greenLed = false;
	mb_yellowLed = false;
	mb_redLed = false;
}

CLeds :: ~CLeds()
{
}

//Turn ON all the LEDs
void CLeds :: outsideZone()
{
	digitalWrite(GREEN_ZONE , HIGH);
	digitalWrite(YELLOW_ZONE , HIGH);
	digitalWrite(RED_ZONE , HIGH);

	mb_greenLed = false;
	mb_yellowLed = false;
	mb_redLed = false;
}

void CLeds :: greenZone()
{
	// Turn on Green LED
	digitalWrite(GREEN_ZONE , HIGH);
	mb_greenLed = true;

	digitalWrite(YELLOW_ZONE, LOW);
	digitalWrite(RED_ZONE, LOW);

	mb_yellowLed = false;
	mb_redLed = false;
}


void CLeds :: yellowZone()
{
	// Turn on Yellow LED
	digitalWrite(YELLOW_ZONE , HIGH);
	mb_yellowLed = true;

	digitalWrite(GREEN_ZONE , LOW);
	digitalWrite(RED_ZONE , LOW);

	mb_greenLed = false;
	mb_redLed = false;
}

void CLeds :: redZone()
{
	// Turn on Red LED
	digitalWrite(RED_ZONE , HIGH);
	mb_redLed = true;

	digitalWrite(GREEN_ZONE , LOW);
	digitalWrite(YELLOW_ZONE , LOW);

	mb_greenLed = false;
	mb_yellowLed = false;
}

void CLeds :: turnOFF()
{
	digitalWrite(GREEN_ZONE , LOW);
	digitalWrite(YELLOW_ZONE , LOW);
	digitalWrite(RED_ZONE , LOW);
}

void CLeds :: batteryWarning()
{
	digitalWrite(PWR_STATUS , HIGH);
	delay(1000);
	digitalWrite(PWR_STATUS , LOW);
	delay(1000);
}


int main()
{
	CLeds led;

	while(1)
	{
		led.outsideZone();
		delay(500);
		led.turnOFF();
		delay(500);
	}

//	led.redZone();
//	led.yellowZone();
//	led.greenZone();

	return 0;
}
