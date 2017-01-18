#ifndef FIELDMAP_H_
#define FIELDMAP_H_

#include "defines.h"

#define DIST_TO_YELLOW_ZONE   20
#define DIST_TO_RED_ZONE      10

using namespace std;

struct SSquare
{
	float lat1;  // first x coordinate
	float long1; // first y coordinate
	float lat2;  // second x coordinate
	float long2; // second y coordinate
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
	bool m_checkInsideGreen(float latitude, float longitude);
	bool m_checkInsideYellow(float latitude, float longitude);
	bool m_checkInsideRed(float latitude, float longitude);
	bool m_checkInsideOut(float latitude, float longitude);
};

#endif /*FIELDMAP_H_*/
