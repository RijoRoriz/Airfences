#ifndef ANIMAL_H_
#define ANIMAL_H_

#include "threadsanimal.h"

#define ANIMAL_CONF_FILE "/root/AnimalConf.txt"

using namespace std;

class CAnimal
{
	private:
		int mi_Zone;
		int mi_ID;
		float mi_Temp;
		float mf_Bat;


	public:
		CAnimal();
		~CAnimal();

		int checkCommand(char *command); //parcing dos comandos
		void saveAnimalConf(); //Save Animal configuations  (ID, FieldID, field coordinates)
		void loadAnimalConf();

		bool saveAnimalInfo();
		bool loadAnimalInfo();
		bool updateAnimalInfo();

};

#endif /*ANIMAL_H_*/
