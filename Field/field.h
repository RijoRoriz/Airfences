#ifndef FIELD_H_
#define FIELD_H_

#include "fieldmap.h"

struct Sanimal
{
	int id;
	int zone;
};

struct SanimalRegist
{
	int id;
	int zone;
	int bat;
	double coorX;
	double coorY;
	float temp;

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
		CField(){};
		~CField(){};
};

#endif /*FIELD_H_*/
