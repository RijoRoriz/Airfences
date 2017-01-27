#include "field.h"

CField::CField(){
  mL_redList=mL_greenList=mL_yellowList=NULL;
  mL_wifiInfoList=NULL;
}

CField::~CField(){
  while(getAnimal(GREENZONE)!=-1);
  while(getAnimal(YELLOWZONE)!=-1);
  while(getAnimal(REDZONE)!=1);
}
void CField::setAnimal(uint16_t ids, int list)
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
      mL_greenList=aux->pointer;
      returnid=aux->id;
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
      mL_yellowList=aux->pointer;
      returnid=aux->id;
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
      mL_redList=aux->pointer;
      returnid=aux->id;
      delete aux;
      return returnid;
    }
  }
}

void CField::setAnimalInfo(char* returnedMsg)
{
  SanimalInfo* aux=NULL;
  aux=mL_wifiInfoList;
  if(!aux)
  {
    aux = new SanimalInfo();
    memcpy(aux->msg,returnedMsg,32);
    aux->pointer = NULL;
    mL_wifiInfoList=aux;
  }
  else
  {
    while(aux->pointer)
    {
      aux=aux->pointer;
    }
    aux->pointer = new SanimalInfo();
    memcpy(aux->msg,returnedMsg,32);
    aux->pointer->pointer = NULL;
  }
}

void CField::getAnimalInfo(char * msg)
{
  SanimalInfo * aux;
  aux=mL_wifiInfoList;
  if(aux==NULL)
  {
    cout << "wifi list empty" << endl;
    msg=NULL;
  }
  else
  {
    mL_wifiInfoList=aux->pointer;
    memcpy(msg,aux->msg,32);
    delete aux;
  }
}
