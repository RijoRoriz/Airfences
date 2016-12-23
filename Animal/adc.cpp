#include "adc.h"

const float mcf_vps4 = 4.096 / 2048.0;
const float mcf_vps2 = 2.048 / 2048.0;

CAdc :: CAdc()
{
	mui_i2cAddress = ADS1015_ADDRESS;

	wiringPiSetup();

	mui_adcHandler = wiringPiI2CSetup(mui_i2cAddress);

	if(mui_adcHandler < 0)
	{
		cout << "Can't find i2c" << endl;
	}

	mui_bitShift = 4;

}

CAdc :: ~CAdc()
{

}


int16_t CAdc :: readADC_SingleEnded(int8_t channel)
{
	int16_t read;

	if (channel > 3)
  {
    return 0;
  }

  // Start with default values
  int16_t config =  ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_LATCH   | // Latching comparator
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config &= ~ADS1015_REG_CONFIG_PGA_MASK;
  config |= ADS1015_REG_CONFIG_PGA_4_096V;
//  config |= ADS1015_REG_CONFIG_PGA_2_048V;

	// Set single-ended input channel
	switch (channel)
  {
    case (0):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;
// cout << "config1: " << hex << config << endl;

  // Write config register to the ADC
  config = swap(config);
  //cout << "config2: " << hex << config << endl;

  wiringPiI2CWriteReg16(mui_adcHandler, ADS1015_REG_POINTER_CONFIG, config);
//	cout << "config3: " << hex << config << endl;


  // Wait for the conversion to complete
  delay(3);

  // Read the conversion results
  read = wiringPiI2CReadReg16 (mui_adcHandler, ADS1015_REG_POINTER_CONVERT);
  read =  swap(read);

  // Shift 12-bit results right 4 bits for the ADS1015
  read = read >> mui_bitShift;

  return read;
}

int16_t CAdc :: swap(int16_t value)
{
	int aux;

	aux = (value & 0x00FF) << 8;
	aux |= (value & 0xFF00) >> 8;

	return aux;
}

void CAdc :: readTemperature()
{
	int read;

	read = readADC_SingleEnded(0);
	//cout << "Read3: " << read << endl;

	mf_tmp36voltage = read * mcf_vps4;

	if(mf_tmp36voltage < MIN_TMP36_OUTPUT || mf_tmp36voltage > MAX_TMP36_OUTPUT)
	{
		readTemperature();
	}
	else
	{
		//Temp °C = 100*(reading in V) - 50
		mf_temperature = 100 * mf_tmp36voltage - 50;

		cout << fixed;
		cout << "Voltage:     " << setprecision(3) << mf_tmp36voltage << endl;
		cout << "Temperature: " << setprecision(3) << mf_temperature  << endl;
	}
}

float CAdc :: getTemperature()
{
	return mf_temperature;
}

float CAdc :: getTMP36Voltage()
{
	return mf_tmp36voltage;
}

void CAdc :: readBatteryLevel()
{
	int read;

	read = readADC_SingleEnded(1);

	mf_batteryLevel = read * mcf_vps4;
	cout << fixed;
	cout << "Voltage: " << setprecision(3) << mf_batteryLevel << endl;
}

float CAdc :: getBatteryLevel()
{
	return mf_batteryLevel;
}

// int main()
// {
//
// 	CAdc adc;
//
// 	cout << "TMP 36" << endl;
// 	adc.readTemperature();
//
//    cout << "Battery Level" << endl;
//    adc.readBatteryLevel();
// //		delay(3000);
//
// 	return 1;
// }
