#include "field.h"
#include <pthread.h>
#include "defines.h"

CField::CField{
  mL_redList=mL_greenList=mL_yellowList=NULL;
}
void CField::setAnimal(int ids, int list)
{
  Sanimal* aux1;
  if(list==GREENZONE)
  {
    aux1=mL_greenList;
    if(aux1==NULL)
    {
      aux1 = new Sanimal();
      aux->id= ids;
    }
    else
    {
      while(aux1->pointer!=NULL)
      {
        aux1=aux1->pointer;
      }
      aux1 = new Sanimal();
      aux->id= ids;
    }

  }
  else if(list==YELLOWZONE)
  {
    aux1=mL_yellowList;
    if(aux1==NULL)
    {
      aux1 = new Sanimal();
      aux->id= ids;
    }
    else
    {
      while(aux1->pointer!=NULL)
      {
        aux1=aux1->pointer;
      }
      aux1 = new Sanimal();
      aux->id= ids;
    }
  }
  else if(list==REDZONE)
  {
    aux1=mL_redList;
    if(aux1==NULL)
    {
      aux1 = new Sanimal();
      aux->id= ids;
    }
    else
    {
      while(aux1->pointer!=NULL)
      {
        aux1=aux1->pointer;
      }
      aux1 = new Sanimal();
      aux->id= ids;
    }
  }
}
int CField::getAnimal(int list)
{
  Sanimal * aux;
  Sanimal * aux2;
  int returnid=-1;
  if(list==GREENZONE)
  {
    aux=mL_greenList;
    if(aux==NULL)
    {
      cout << "greenList empty" << endl;
      return -1;
    }
    else
    {
      aux2=aux->pointer;
      returnid=aux->id;
      mL_greenList=aux2;
      delete aux;
      return returnid;
    }
  }
  else if(list==YELLOWZONE)
  {
    aux=mL_yellowList;
    if(aux==NULL)
    {
      cout << "yellowList empty" << endl;
      return -1;
    }
    else
    {
      aux2=aux->pointer;
      mL_yellowList=aux2;
      delete aux;
      return returnid;
    }
  }
  else if(list==REDZONE)
  {
    aux=mL_redList;
    if(aux==NULL)
    {
      cout << "redList empty" << endl;
      return -1;
    }
    else
    {
      aux2=aux->pointer;
      mL_redList=aux2;
      delete aux;
      return returnid;
    }
  }

}
