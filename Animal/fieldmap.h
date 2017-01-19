#ifndef FIELDMAP_H_
#define FIELDMAP_H_

#include "defines.h"

#define DIST_TO_YELLOW_ZONE   20
#define DIST_TO_RED_ZONE      10

using namespace std;

struct SSquare
{
	float lat1;
	float long1;
	float lat2;
	float long2;
};

class CFieldMap
{
private:
	struct SSquare m_greenzone;
	struct SSquare m_yellowzone;
	struct SSquare m_redzone;
	struct SSquare m_outzone;

public:
	CFieldMap();
	~CFieldMap();

	void m_configureMap(struct SSquare greenZone);
	int mi_checkAnimalZone(float latitude, float longitude);

};

#endif /*FIELDMAP_H_*/
