#include "fieldmap.h"

CFieldMap :: CFieldMap()
{

}

CFieldMap :: ~CFieldMap()
{

}

void CFieldMap :: m_configureMap(struct SSquare greenZone)
{
  m_greenzone.lat1 = greenZone.lat1;
  m_greenzone.long1 = greenZone.long1;
  m_greenzone.lat2 = greenZone.lat2;
  m_greenzone.long2 = greenZone.long2;

  m_yellowzone.lat1 = greenZone.lat1 + DIST_TO_YELLOW_ZONE;
  m_yellowzone.long1 = greenZone.long1 + DIST_TO_YELLOW_ZONE;
  m_yellowzone.lat2 = greenZone.lat2 + DIST_TO_YELLOW_ZONE;
  m_yellowzone.long2 = greenZone.long2 + DIST_TO_YELLOW_ZONE;

  m_redzone.lat1 = greenZone.lat1 + DIST_TO_RED_ZONE;
  m_redzone.long1 = greenZone.long1 + DIST_TO_RED_ZONE;
  m_redzone.lat2 = greenZone.lat2 + DIST_TO_RED_ZONE;
  m_redzone.long2 = greenZone.long2 + DIST_TO_RED_ZONE;

  m_outzone.lat1 = m_redzone.lat1;
  m_outzone.long1 = m_redzone.long1;
  m_outzone.lat2 = m_redzone.lat2;
  m_outzone.long2 = m_redzone.long2;
}

int CFieldMap :: mi_checkAnimalZone(float latitude, float longitude)
{
  if((m_greenzone.lat1 > latitude) && (m_greenzone.lat2 < latitude)
  && (m_greenzone.long1 > longitude) && (m_greenzone.long2 < longitude)) {
    return GREENZONE;
  }
  else if((m_yellowzone.lat1 > latitude) && (m_yellowzone.lat2 < latitude)
  && (m_yellowzone.long1 > longitude) && (m_yellowzone.long2 < longitude)) {
    return YELLOWZONE;
  }
  else if((m_redzone.lat1 > latitude) && (m_redzone.lat2 < latitude)
  && (m_redzone.long1 > longitude) && (m_redzone.long2 < longitude)) {
    return REDZONE;
  }
  else if((m_redzone.lat1 < latitude) || (m_redzone.lat2 > latitude)
  || (m_redzone.long1 < longitude) || (m_redzone.long2 > longitude)) {
    return OUTZONE;
  }
  else {
    return -1;
  }
}
