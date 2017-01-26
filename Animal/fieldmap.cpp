#include "fieldmap.h"

CFieldMap :: CFieldMap()
{

}

CFieldMap :: ~CFieldMap()
{

}

void CFieldMap :: m_configureMap(SSquare fenceLimits)
{
  float dLat, dLong;

  dLat = fenceLimits.lat2 - fenceLimits.lat1;
  dLong = fenceLimits.long2 - fenceLimits.long1;

  m_redzone.lat1 = fenceLimits.lat1;
  m_redzone.long1 = fenceLimits.long1;
  m_redzone.lat2 = fenceLimits.lat2;
  m_redzone.long2 = fenceLimits.long2;

  m_yellowzone.lat1 = fenceLimits.lat1 + dLat * DEG_YELLOW_ZONE;
  m_yellowzone.long1 = fenceLimits.long1 + dLong * DEG_YELLOW_ZONE;
  m_yellowzone.lat2 = fenceLimits.lat2 - dLat * DEG_YELLOW_ZONE;
  m_yellowzone.long2 = fenceLimits.long2 - dLong * DEG_YELLOW_ZONE;

  m_greenzone.lat1 = fenceLimits.lat1 + dLat * DEG_GREEN_ZONE;
  m_greenzone.long1 = fenceLimits.long1 + dLong * DEG_GREEN_ZONE;
  m_greenzone.lat2 = fenceLimits.lat2 - dLat * DEG_GREEN_ZONE;
  m_greenzone.long2 = fenceLimits.long2 - dLong * DEG_GREEN_ZONE;
}

int CFieldMap :: mi_checkAnimalZone(float latitude, float longitude)
{
  if((latitude > m_greenzone.lat1) && (latitude < m_greenzone.lat2)
  && (longitude > m_greenzone.long1) && (longitude < m_greenzone.long2)) {
    return GREENZONE;
  }
  else if((latitude > m_yellowzone.lat1) && (latitude < m_yellowzone.lat2)
  && (longitude > m_yellowzone.long1) && (longitude < m_yellowzone.long2)) {
    return YELLOWZONE;
  }
  else if((latitude > m_redzone.lat1) && (latitude < m_redzone.lat2)
  && (longitude > m_redzone.long1) && (longitude < m_redzone.long2)) {
    return REDZONE;
  }
  else if((latitude > m_redzone.lat1) || (latitude < m_redzone.lat2)
  || (longitude > m_redzone.long1) || (longitude < m_redzone.long2)) {
    return OUTZONE;
  }
  else {
    return NOZONE;
  }
}

void CFieldMap :: m_printFieldZones()
{
  cout << setprecision(6) << fixed
  << "GREEN ZONE:  " << m_greenzone.lat1 << ' ' << m_greenzone.long1 << ' '
  << m_greenzone.lat2 << ' ' << m_greenzone.long2 << endl
  << "YELLOW ZONE: " << m_yellowzone.lat1 << ' ' << m_yellowzone.long1 << ' '
  << m_yellowzone.lat2 << ' ' << m_yellowzone.long2 << endl
  << "RED ZONE:    " << m_redzone.lat1 << ' ' << m_redzone.long1 << ' '
  << m_redzone.lat2 << ' ' << m_redzone.long2 << endl;
}
