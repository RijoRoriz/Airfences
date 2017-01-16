#ifndef FIELDMAP_H_
#define FIELDMAP_H_

#include "defines.h"

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

	void m_calcGreenzone();
	void m_calcYellowzone();
	void m_calcRedzone();

	public:
	CFieldMap();
	~CFieldMap();

	int m_configureMap(struct SSquare);
	bool m_checkInsideGreen(float latitude, float longitude);
	bool m_checkInsideYellow(float latitude, float longitude);
	bool m_checkInsideRed(float latitude, float longitude);
	bool m_checkInsideOut(float latitude, float longitude);
};

#endif /*FIELDMAP_H_*/
