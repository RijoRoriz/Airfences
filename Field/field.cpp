#include "field.h"

CField::CField(){
  mL_redList=mL_greenList=mL_yellowList=NULL;
  mL_wifiInfoList=NULL;
}

CField::~CField(){
  while(getAnimal(GREENZONE)!=0);
  while(getAnimal(YELLOWZONE)!=0);
  while(getAnimal(REDZONE)!=0);
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
uint16_t CField::getAnimal(int list)
{
  Sanimal * aux;
  int returnid=-1;
  if(list==GREENZONE)
  {
    aux=mL_greenList;
    if(aux==NULL)
    {
      return 0;
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
      return 0;
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
      return 0;
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

uint16_t CField::getNextAnimal()
{
  uint16_t idAnimal;

  idAnimal = getAnimal(REDZONE);
  if((idAnimal != 0) && (mi_count_Red < 3)) {
    mi_count_Red++;
    return idAnimal;
  }
  else {
    mi_count_Red = 0;
  }

  idAnimal = getAnimal(YELLOWZONE);
  if((idAnimal != 0) && (mi_count_Yellow < 3)) {
    mi_count_Yellow++;
    return idAnimal;
  }
  else {
    mi_count_Yellow = 0;
  }

  idAnimal = getAnimal(GREENZONE);
  if((idAnimal != 0) && (mi_count_Green < 3)) {
    mi_count_Green++;
    return idAnimal;
  }
  else {
    mi_count_Green = 0;
    return 0;
  }
}

void CField::setAnimalInfo(unsigned char* returnedMsg)
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

void CField::getAnimalInfo(unsigned char * msg)
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
