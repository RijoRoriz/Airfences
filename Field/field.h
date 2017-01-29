#ifndef FIELD_H_
#define FIELD_H_

#include "fieldmap.h"
#include "defines.h"


struct Sanimal
{
	uint16_t id;
	Sanimal * pointer;
};

struct SanimalInfo
{
	char msg[32];
	SanimalInfo * pointer;
};

class CField
{
	private:
		CFieldMap fieldmap;
		struct Sanimal *mL_greenList;
		struct Sanimal *mL_yellowList;
		struct Sanimal *mL_redList;
		struct SanimalRegist *mL_sendInfoList;
		struct SanimalInfo *mL_wifiInfoList;
		uint16_t idField;

	public:
		CField();
		~CField();
		void setAnimal(uint16_t id, int list);
	 	int getAnimal(int list);
		void setAnimalInfo(char *msg);
		void getAnimalInfo(char* returnedMsg);
		void set_idAnimal(uint16_t);
		uint16_t get_idAnimal();
		void set_idField(uint16_t);
};

#endif /*FIELD_H_*/
