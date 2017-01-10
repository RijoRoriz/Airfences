#include "animal.h"

CAnimal :: CAnimal()
{

}

CAnimal :: ~CAnimal()
{

}

int CAnimal :: checkCommand(char *command)
{
  char buffer[50];
  int ifieldID, ianimalID;
  char ccommandType;

  memset(buffer, '\0', 50);

  strcpy(buffer, command);

  sscanf(buffer, "%d,%d,%c", &ifieldID, &ianimalID, &ccommandType);

  switch (ccommandType)
  {
    case 'R': //Reset "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
      int iresetTemp, iresetBattery, iresetGPS, iresetRF, istate;

      sscanf(buffer, "%d,%d,%c,%d,%d,%d,%d,%d", &ifieldID, &ianimalID, &ccommandType, &iresetTemp, &iresetBattery, &iresetGPS, &iresetRF, &istate);

      if(iresetTemp){

      }
      else if(iresetBattery){

      }
      else if(iresetGPS){

      }
      else if(iresetRF){

      }
      else{

      }

      break;

    case 'I': //Field request animal info "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
    int ireqTemp, ireqBattery, ireqGPS, ireqRF, istate;

    sscanf(buffer, "%d,%d,%c,%d,%d,%d,%d,%d", &ifieldID, &ianimalID, &ccommandType, &ireqTemp, &ireqBattery, &ireqGPS, &ireqRF, &istate);

    if(ireqTemp){

    }
    else if(ireqBattery){

    }
    else if(ireqGPS){

    }
    else if(ireqRF){

    }
    else{

    }




    break;

    case 'N': //First Configuration "ID_Field,ID_Animal,N,ID_Animal,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
    break;

    case 'C': //New Configuration "ID_Field,ID_Animal,C,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
    break;
  }


  return 0;
}

void CAnimal :: saveAnimalConf()
{
  ofstream outfile;

  outfile.open(ANIMAL_CONF_FILE, outfile.out); //writing

  if(outfile.is_open() && outfile.good()){ //Save Animal Configurations

  }
  else{
    cout << "Error Saving File: " << ANIMAL_CONF_FILE << endl;
  }
  outfile.close();
}

void CAnimal :: loadAnimalConf()
{
  ifstream infile;
  char buffer[50];

  infile.open(ANIMAL_CONF_FILE, infile.in); //reading

  if(infile.is_open() && infile.good()){
    while(!infile.eof())
    {
      getline(infile, buffer);
    }
    infile.close();
  }
  else {
    cout << "Error Loading File: " << ANIMAL_CONF_FILE << endl;
  }

}

bool CAnimal :: saveAnimalInfo()
{

}

bool CAnimal :: loadAnimalInfo()
{

}

bool CAnimal :: updateAnimalInfo()
{

}
