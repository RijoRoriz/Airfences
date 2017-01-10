#ifndef ANIMAL_H_
#define ANIMAL_H_

#include "threadsanimal.h"

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

		bool saveAnimalInfo();
		bool loadAnimalInfo();
		bool updateAnimalInfo();

};

#endif /*ANIMAL_H_*/
