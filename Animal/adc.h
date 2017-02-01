#include "defines.h"

#define MAX_BAT_LEVEL    3.17
#define MAX_DIF_VOLTAGE  0.43

using namespace std;

class CAdc
{
	private:
		int8_t mui_adcHandler;
		int8_t mui_i2cAddress;
		int8_t mui_bitShift;

		float mf_tmp36voltage;
		float mf_temperature;
		float mf_batteryLevel;

		int16_t swap(int16_t value);

	public:
		CAdc();
		~CAdc();

		int16_t readADC_SingleEnded(int8_t channel);

		void readTemperature();
		float getTemperature();
		float getTMP36Voltage();

		void readBatteryLevel();
		float getBatteryLevel();

};
