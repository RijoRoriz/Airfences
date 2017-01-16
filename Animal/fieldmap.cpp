#include "fieldmap.h"

CFieldMap :: CFieldMap()
{

}

CFieldMap :: ~CFieldMap()
{

}

int CFieldMap :: m_configureMap(struct SSquare)
{
  return 0;

}

bool CFieldMap :: m_checkInsideGreen(float latitude, float longitude)
{
  return true;
}

bool CFieldMap :: m_checkInsideYellow(float latitude, float longitude)
{
  return true;
}

bool CFieldMap :: m_checkInsideRed(float latitude, float longitude)
{
  return true;
}

bool CFieldMap :: m_checkInsideOut(float latitude, float longitude)
{
  return true;
}
