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

bool CFieldMap :: m_checkInsideGreen(long, long)
{
  return true;
}

bool CFieldMap :: m_checkInsideYellow(long, long)
{
  return true;
}

bool CFieldMap :: m_checkInsideRed(long, long)
{
  return true;
}

bool CFieldMap :: m_checkInsideOut(long, long)
{
  return true;
}
