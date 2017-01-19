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

	public:
		CLeds();
		~CLeds();

		void m_turnON_LedZone(int zone);
		void m_turnOFF();
		void m_batteryWarning();

};

#endif
