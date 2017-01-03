#ifndef FIELDMAP_H_
#define FIELDMAP_H_

#include "defines.h"

using namespace std;

struct SSquare
{
	long ml_x1; // first x coordinate
	long ml_y1; // first y coordinate
	long ml_x2; // second x coordinate
	long ml_y2; // second y coordinate
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
	bool m_checkInsideGreen(long, long);
	bool m_checkInsideYellow(long, long);
	bool m_checkInsideRed(long, long);
	bool m_checkInsideOut(long, long);
};

#endif /*FIELDMAP_H_*/
