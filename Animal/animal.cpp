#include "animal.h"

CAnimal :: CAnimal()
{

}

CAnimal :: ~CAnimal()
{

}

char * CAnimal :: checkCommand(char *command)
{
  int ifieldID, ianimalID;
  char ccommandType;
  char cState;

  char resetCommand[4]; // 0-TYPE, 1-TEMP, 2-BAT, 3-GPS, 4-RF
  char requestCommand[4]; // 0-TYPE, 1-TEMP, 2-BAT, 3-GPS, 4-RF

  mmset(resetCommand, '\0', 4);
  mmset(requestCommand, '\0', 4);

  sscanf(command, "%d,%d,%c", &ifieldID, &ianimalID, &ccommandType);

  switch (command[2])
  {
    case 'R': //Reset "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
      sscanf(command, "%d,%d,%c,%c,%c,%c,%c,%c", &ifieldID, &ianimalID, &resetCommand[0], &resetCommand[1], &resetCommand[2], &resetCommand[3], &resetCommand[4], &cState);

      return resetCommand;
      break;

    case 'I': //Field request animal info "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
      sscanf(command, "%d,%d,%c,%c,%c,%c,%c,%c", &ifieldID, &ianimalID, &requestCommand[0], &requestCommand[1], &requestCommand[2], &requestCommand[3], &requestCommand[4],  &cState);

      return requestCommand;
      break;

    case 'N': //First Configuration "ID_Field,ID_Animal,N,ID_Animal,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
    SSquare greenZone;
    sscanf(command, "%d,%d,%c,%f,%f,%f,%f", &ifieldID, &ianimalID, &ccommandType, &greenZone.x1, &greenZone.x2, &greenZone.y1, &greenZone.y2);



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
