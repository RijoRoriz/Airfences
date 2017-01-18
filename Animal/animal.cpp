#include "animal.h"

CAnimal :: CAnimal()
{
  mi_Zone = 0;


}

CAnimal :: ~CAnimal()
{

}

void CAnimal :: setAnimalTimeout(int zone)
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

int CAnimal :: getAnimalTimeout()
{
  return mi_timeout;
}

void CAnimal :: setAnimalZone(int zone)
{
  mi_Zone = zone;
}

int CAnimal :: getAnimalZone()
{
  return mi_Zone;
}

char * CAnimal :: checkCommand(char *command)
{
  // int ifieldID, ianimalID;
  // char ccommandType;
  // char cState;
  //
  // char resetCommand[4]; // 0-TYPE, 1-TEMP, 2-BAT, 3-GPS, 4-RF
  // char requestCommand[4]; // 0-TYPE, 1-TEMP, 2-BAT, 3-GPS, 4-RF
  //
  // memset(resetCommand, '\0', 4);
  // memset(requestCommand, '\0', 4);
  //
  // sscanf(command, "%d,%d,%c", &ifieldID, &ianimalID, &ccommandType);
  //
  // switch (command[2])
  // {
  //   case 'R': //Reset "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
  //     sscanf(command, "%d,%d,%c,%c,%c,%c,%c,%c", &ifieldID, &ianimalID, &resetCommand[0], &resetCommand[1], &resetCommand[2], &resetCommand[3], &resetCommand[4], &cState);
  //
  //     return resetCommand;
  //     break;
  //
  //   case 'I': //Field request animal info "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
  //     sscanf(command, "%d,%d,%c,%c,%c,%c,%c,%c", &ifieldID, &ianimalID, &requestCommand[0], &requestCommand[1], &requestCommand[2], &requestCommand[3], &requestCommand[4],  &cState);
  //
  //     return requestCommand;
  //     break;
  //
  //   case 'N': //First Configuration "ID_Field,ID_Animal,N,ID_Animal,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
  //   SSquare greenZone;
  //   sscanf(command, "%d,%d,%c,%f,%f,%f,%f", &ifieldID, &ianimalID, &ccommandType, &greenZone.x1, &greenZone.x2, &greenZone.y1, &greenZone.y2);
  //
  //
  //
  //   break;
  //
  //   case 'C': //New Configuration "ID_Field,ID_Animal,C,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
  //   break;
  // }


  return 0;
}

//First time that the configs are saved
void CAnimal :: setAnimalConf(unsigned char* message)
{
  uint16_t idAnimalFabric;
  int16_t aux_degrees;
  uint32_t aux_degrees_dec;
  float totalDegrees;

  SSquare greenZone;

  //message[0-1] - ID_Field
  mui_idField = ((message[0] << 8) | message[1]);

  //message[2-3] - Fabric ID_Animal = 0
  idAnimalFabric = ((message[2] << 8) | message[3]);

  //message[5-6] - ID_Animal to set
  mui_idAnimal = ((message[5] << 8) | message[6]);

  //COORDINATES OF THE GREEN ZONE
  //message[7-11] - latitude 1 greenZone
  aux_degrees = message[7]; //Graus -90º a 90º
  aux_degrees_dec = (message[8] << 24 | message[9] << 16 | message[10] << 8 | message[11]);

  totalDegrees = (abs(aux_degrees) + static_cast<float>(aux_degrees_dec)/100000);

  if(aux_degrees < 0) {
    totalDegrees *= -1;
  }
  else {
    totalDegrees *= 1;
  }

  greenZone.lat1 = totalDegrees;

  //message[12-16] - latitude 2 greenZone
  aux_degrees = message[12]; //Graus -90º a 90º
  aux_degrees_dec = (message[13] << 24 | message[14] << 16 | message[15] << 8 | message[16]);

  totalDegrees = (abs(aux_degrees) + static_cast<float>(aux_degrees_dec)/100000);

  if(aux_degrees < 0) {
    totalDegrees *= -1;
  }
  else {
    totalDegrees *= 1;
  }

  greenZone.lat2 = totalDegrees;

  //message[17-22] - longitude 1 greenZone
  aux_degrees = ((message[17] << 8) | message[18]); //graus -180º a 180º
  aux_degrees_dec = (message[19] << 24 | message[20] << 16 | message[21] << 8 | message[22]);

  totalDegrees = (abs(aux_degrees) + static_cast<float>(aux_degrees_dec)/100000);

  if(aux_degrees < 0) {
    totalDegrees *= -1;
  }
  else {
    totalDegrees *= 1;
  }

  greenZone.long1 = totalDegrees;

  //message[23-28] - longitude 2 greenZone
  aux_degrees = ((message[23] << 8) | message[24]); //graus -180º a 180º
  aux_degrees_dec = (message[25] << 24 | message[26] << 16 | message[27] << 8 | message[28]);

  totalDegrees = (abs(aux_degrees) + static_cast<float>(aux_degrees_dec)/100000);

  if(aux_degrees < 0) {
    totalDegrees *= -1;
  }
  else {
    totalDegrees *= 1;
  }

  greenZone.long2 = totalDegrees;
}

void CAnimal :: saveAnimalConf()
{
  // ofstream outfile;
  //
  // outfile.open(ANIMAL_CONF_FILE, outfile.out); //writing
  //
  // if(outfile.is_open() && outfile.good()){ //Save Animal Configurations
  //
  // }
  // else{
  //   cout << "Error Saving File: " << ANIMAL_CONF_FILE << endl;
  // }
  // outfile.close();
}

void CAnimal :: loadAnimalConf()
{
  // ifstream infile;
  // char buffer[50];
  //
  // infile.open(ANIMAL_CONF_FILE, infile.in); //reading
  //
  // if(infile.is_open() && infile.good()){
  //   while(!infile.eof())
  //   {
  //     getline(infile, buffer);
  //   }
  //   infile.close();
  // }
  // else {
  //   cout << "Error Loading File: " << ANIMAL_CONF_FILE << endl;
  // }

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
