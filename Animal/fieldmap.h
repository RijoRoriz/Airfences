struct SSquare
{
	long ml_x1; // first x coordinate
	long ml_y1; // first y coordinate
	long ml_x2; // second x coordinate
	long ml_y2; // second y coordinate
};

class CfieldMap
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
	CfieldMap();
	~CfieldMap();
	int m_configureMap(struct SSquare);
	bool m_checkInsideGreen(long, long);
	bool m_checkInsideYellow(long, long);
	bool m_checkInsideRed(long, long);
	bool m_checkInsideOut(long, long);	
};
