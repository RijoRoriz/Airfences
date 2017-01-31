#include "animal.h"

CAnimal :: CAnimal()
{
  mui_idAnimal = 0;
  mui_idField = 0;
  mi_timeout = TIME_RED_ZONE;
  mi_Zone = NOZONE;

  //Load file with animal confs
  m_loadAnimalConf();
}

CAnimal :: ~CAnimal()
{

}

uint16_t CAnimal :: mui_getAnimalID()
{
  return mui_idAnimal;
}

void CAnimal :: m_setAnimalTimeout(int zone)
{
  switch(zone)
  {
    case GREENZONE:
    mi_timeout = TIME_GREEN_ZONE;
    break;

    case YELLOWZONE:
    mi_timeout = TIME_YELLOW_ZONE;
    break;

    case REDZONE:
    mi_timeout = TIME_RED_ZONE;
    break;
    //OUTZONE
    default:
    mi_timeout = TIME_RED_ZONE;
    break;
  }
}

int CAnimal :: mi_getAnimalTimeout()
{
  return mi_timeout;
}

void CAnimal :: m_setAnimalZone(int zone)
{
  mi_Zone = zone;
}

int CAnimal :: mi_getAnimalZone()
{
  return mi_Zone;
}

void CAnimal :: m_setAnimalConf(unsigned char* message)
{
  uint16_t idAnimalFabric, idField, idAnimal;

  switch (message[4])
  {
    //First Config "ID_Field,ID_Animal,N,ID_Animal,FenceLimit_x1,FenceLimit_x2,FenceLimit_y1,FenceLimit_y2"
    case 'N':
    memcpy(&mui_idField, &message[0], 2); //Set FieldID
    memcpy(&idAnimalFabric, &message[2], 2);

    if(idAnimalFabric == mui_idAnimal) { //idAnimalFabric = 0
      memcpy(&mui_idAnimal, &message[5], 2); //Set AnimalID
    }
    //Set the greenZone
    memcpy(&m_fenceLimits.lat1, &message[7], 4);
    memcpy(&m_fenceLimits.lat2, &message[11], 4);
    memcpy(&m_fenceLimits.long1, &message[15], 4);
    memcpy(&m_fenceLimits.long2, &message[19], 4);

    m_saveAnimalConf();
    break;

    //New Config "ID_Field,ID_Animal,C,FenceLimit_x1,FenceLimit_x2,FenceLimit_y1,FenceLimit_y2"
    case 'C':
    memcpy(&idField, &message[0], 2);
    memcpy(&idAnimal, &message[2], 2);

    if((idField == mui_idField) && (idAnimal == mui_idAnimal)) {
      //Set the greenZone
      memcpy(&m_fenceLimits.lat1, &message[5], 4);
      memcpy(&m_fenceLimits.lat2, &message[9], 4);
      memcpy(&m_fenceLimits.long1, &message[13], 4);
      memcpy(&m_fenceLimits.long2, &message[17], 4);

      m_saveAnimalConf();
    }
    break;
  }
}

SSquare CAnimal :: mssq_getAnimalFenceLimits()
{
  return m_fenceLimits;
}

void CAnimal :: m_saveAnimalConf()
{
  ofstream outfile;

  outfile.open(ANIMAL_CONF_FILE, outfile.out); //writing

  if(outfile.is_open() && outfile.good()){ //Save Animal Configurations
    outfile << mui_idField << ';' << mui_idAnimal << ';'
      << m_fenceLimits.lat1 << ';' << m_fenceLimits.lat2 << ';'
      << m_fenceLimits.long1 << ';' << m_fenceLimits.long2;
  }
  else{
    perror("CAnimal::m_saveAnimalConf In infile.open()");
  }
  outfile.close();
}

void CAnimal :: m_loadAnimalConf()
{
  ifstream infile;
  string animalConf;

  infile.open(ANIMAL_CONF_FILE, infile.in); //reading

  if(infile.is_open() && infile.good()){
    while(!infile.eof())
    {
      getline(infile, animalConf);

    sscanf(&animalConf[0], "%hu;%hu;%f;%f;%f;%f", &mui_idField, &mui_idAnimal, &m_fenceLimits.lat1, &m_fenceLimits.lat2, &m_fenceLimits.long1, &m_fenceLimits.long2);
    }
    infile.close();

    #if DEBUG
    cout << setprecision(6) << fixed
    << "ID Field: " << mui_idField << endl
    << "ID Animal: " << mui_idAnimal << endl
    << "Lat1: " << m_fenceLimits.lat1 << endl
    << "Long1: " << m_fenceLimits.long1 << endl
    << "Lat2: " << m_fenceLimits.lat2 << endl
    << "Long2: " << m_fenceLimits.long2 << endl;
    #endif
  }
  else {
    perror("CAnimal::m_loadAnimalConf In infile.open()");
  }
}
