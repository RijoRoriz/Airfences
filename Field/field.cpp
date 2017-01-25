#include "field.h"
#include <pthread.h>
#include "defines.h"

CField::CField(){
  mL_redList=mL_greenList=mL_yellowList=NULL;
}

CField::~CField(){
  while(getAnimal(GREENZONE)!=-1);
  while(getAnimal(YELLOWZONE)!=-1);
  while(getAnimal(REDZONE)!=1);
}
void CField::setAnimal(int ids, int list)
{
  Sanimal* aux=NULL;
  if(list==GREENZONE)
  {
    aux=mL_greenList;
    if(!aux)
    {
      aux = new Sanimal();
      aux->id= ids;
      aux->pointer = NULL;
      mL_greenList=aux;
    }
    else
    {
      while(aux->pointer)
      {
        aux=aux->pointer;
      }
      aux->pointer = new Sanimal();
      aux->pointer->id= ids;
      aux->pointer->pointer = NULL;
    }

  }
  else if(list==YELLOWZONE)
  {
    aux=mL_yellowList;
    if(!aux)
    {
      aux = new Sanimal();
      aux->id= ids;
      aux->pointer = NULL;
      mL_yellowList=aux;
    }
    else
    {
      while(aux->pointer)
      {
        aux=aux->pointer;
      }
      aux->pointer = new Sanimal();
      aux->pointer->id= ids;
      aux->pointer->pointer = NULL;
    }
  }
  else if(list==REDZONE)
  {
    aux=mL_redList;
    if(!aux)
    {
      aux = new Sanimal();
      aux->id= ids;
      aux->pointer = NULL;
      mL_redList=aux;
    }
    else
    {
      while(aux->pointer)
      {
        aux=aux->pointer;
      }
      aux->pointer = new Sanimal();
      aux->pointer->id= ids;
      aux->pointer->pointer = NULL;
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
