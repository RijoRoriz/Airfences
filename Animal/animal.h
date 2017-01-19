#ifndef ANIMAL_H_
#define ANIMAL_H_

#include "threadsanimal.h"

#define ANIMAL_CONF_FILE "/root/AnimalConf.txt"

#define TIME_GREEN_ZONE   30
#define TIME_YELLOW_ZONE  20
#define TIME_RED_ZONE     10

using namespace std;

struct SSquare
{
	float lat1;  // first x coordinate
	float long1; // first y coordinate
	float lat2;  // second x coordinate
	float long2; // second y coordinate
};

class CAnimal
{
	private:
		int mi_Zone;

		uint16_t mui_idAnimal;
		uint16_t mui_idField;

		SSquare map_greenZone;

		int mi_timeout;
		//float mi_Temp;
		//float mf_Bat;


	public:
		CAnimal();
		~CAnimal();

		void setAnimalTimeout(int zone);
		int getAnimalTimeout();

		void setAnimalZone(int zone);
		int getAnimalZone();

		void setAnimalConf(unsigned char* message);
		SSquare getAnimalGreenZone();
		void saveAnimalConf(); //Save Animal configuations  (ID, FieldID, field coordinates)
		void loadAnimalConf();

		bool saveAnimalInfo();
		bool loadAnimalInfo();
		bool updateAnimalInfo();

};

#endif /*ANIMAL_H_*/
