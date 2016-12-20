#ifndef LEDS_H__
#define LEDS_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>


//      Label     wiringPi Pin
#define GREEN_ZONE   3 // GREEN LED
#define YELLOW_ZONE  2 // YELLOW LED
#define RED_ZONE     0 // RED LED

#define PWR_STATUS    4 // RED LED



class CLeds
{
	private:
		bool mb_greenLed;
		bool mb_yellowLed;
		bool mb_redLed;

	public:
		CLeds();
		~CLeds();

		void outsideZone();
		void greenZone();
		void yellowZone();
		void redZone();

		void turnOFF();

		void batteryWarning();

};

#endif
