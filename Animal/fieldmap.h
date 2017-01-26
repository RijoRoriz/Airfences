#ifndef FIELDMAP_H_
#define FIELDMAP_H_

#include "defines.h"

#define DEG_YELLOW_ZONE    1/16
#define DEG_GREEN_ZONE     3/16

using namespace std;

class CFieldMap
{
private:
	SSquare m_greenzone;
	SSquare m_yellowzone;
	SSquare m_redzone;

public:
	CFieldMap();
	~CFieldMap();

	void m_configureMap(SSquare fenceLimits);
	int mi_checkAnimalZone(float latitude, float longitude);
	void m_printFieldZones();

};

#endif /*FIELDMAP_H_*/
