#ifndef LEDS_H__
#define LEDS_H__

#include "defines.h"

//      Label     wiringPi Pin
#define LED_GREEN_ZONE   3 // GREEN LED
#define LED_YELLOW_ZONE  2 // YELLOW LED
#define LED_RED_ZONE     0 // RED LED

#define LED_PWR_STATUS    4 // RED LED


class CLeds
{
	private:

	public:
		CLeds();
		~CLeds();

		void m_turnON_LedZone(int zone);
		void m_turnOFF();
		void m_BatteryLedStatus(bool state);
		void m_batteryWarning();

};

#endif
