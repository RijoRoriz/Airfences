#ifndef ANIMAL_H_
#define ANIMAL_H_

#include "threadsanimal.h"
#include <signal.h>
#include <time.h>

#define ANIMAL_CONF_FILE "/root/AnimalConf.csv"

#define TIME_GREEN_ZONE   30
#define TIME_YELLOW_ZONE  20
#define TIME_RED_ZONE     10

using namespace std;

class CAnimal
{
	private:
		int mi_Zone;

		uint16_t mui_idAnimal;
		uint16_t mui_idField;

		SSquare m_fenceLimits;

		int mi_timeout;
		//float mi_Temp;
		//float mf_Bat;

	public:
		CAnimal();
		~CAnimal();

		void m_setAnimalTimeout(int zone);
		int mi_getAnimalTimeout();

		void m_setAnimalZone(int zone);
		int mi_getAnimalZone();

		void m_setAnimalConf(unsigned char* message);
		SSquare mssq_getAnimalFenceLimits();
		void m_saveAnimalConf(); //Save Animal configuations  (ID, FieldID, field coordinates)
		void m_loadAnimalConf();

		//bool saveAnimalInfo();
		//bool loadAnimalInfo();

};

#endif /*ANIMAL_H_*/
