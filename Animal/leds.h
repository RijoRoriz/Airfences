#ifndef LEDS_H_
#define LEDS_H_

class Cleds
{
	private:
		bool mb_redLed;
		bool mb_greenLed;
		bool mb_yellowLed;
	public:
		Cled();
		~Cled();
		
		void outsideZone();
		void greenZone();
		void yellowZone();
		void redZone();
		void batteryWarning();
				
};
#endif /*LEDS_H_*/
