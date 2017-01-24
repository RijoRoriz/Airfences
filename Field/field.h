#ifndef FIELD_H_
#define FIELD_H_

#include "fieldmap.h"

struct Sanimal
{
	int id;
	Sanimal * pointer;
};

class CField
{
	private:
		CFieldMap fieldmap;
		struct Sanimal *mL_greenList;
		struct Sanimal *mL_yellowList;
		struct Sanimal *mL_redList;
		struct SanimalRegist *mL_sendInfoList;
	public:

		CField();
		~CField();
		void setAnimal(int id, int list);
	 	int getAnimal(int list);
};

#endif /*FIELD_H_*/
