#include "gps.h"

CGps :: CGps()
{
  gpsHandler = serialOpen("/dev/ttyAMA0", 115200);
  //gpsHandler = serialOpen("/dev/ttyUSB0", 115200);

  wiringPiSetup();

  pinMode(PWR_GPS, OUTPUT);
  digitalWrite(PWR_GPS, LOW);

  if(gpsHandler < 0 || wiringPiSetup() == -1)
  {
		cout << "Error GPS" << endl;
  }

  mb_gpsPower = false;
  mb_gpsData = false;

}

CGps :: ~CGps()
{
	serialClose(gpsHandler);
}

void CGps :: powerON()
{
	int8_t answer = 0;

	answer = sendATCommand("ATE0\r\n", "OK", 2000); //Check if the module is started

	if(answer == 0) //Power ON the GPS module
	{
		digitalWrite(PWR_GPS, HIGH);
		delay(2000);
		digitalWrite(PWR_GPS, LOW);

		//Wait for an answer from the module
		while(answer == 0)
		{
			answer = sendATCommand("ATE0\r\n", "OK", 2000);
		}
	}
}

void CGps :: powerOFF()
{
	sendATCommand("AT+CPOWD=1\r\n", "OK", 2000);
  mb_gpsPower = false;
}


void CGps :: initGps()
{
  int i = 0;

	powerON();

	//sendATCommand("ATE0\r\n", "OK", 2000);

	sendATCommand("AT+CGPSPWR=1\r\n", "OK",2000);

	sendATCommand("AT+CGPSRST=0\r\n", "OK", 2000);

	// waits for fix GPS
	while( !(sendATCommand("AT+CGPSSTATUS?\r\n", "2D Fix", 3000) ||
			sendATCommand("AT+CGPSSTATUS?\r\n", "3D Fix", 3000)))
     {
     	cout << i << "->Fixing GPS: Location Unknown" << endl;
      i++;
     }

     mb_gpsPower = true;
}


unsigned int CGps :: sendATCommand(const char *AT, const char *expected_answer, unsigned int timeout)
{
    char answer[100];
    int i=0;
    int8_t flag=0;
    unsigned long previous;

    memset(answer, '\0', 100);    // Initialize the string

    delay(100);

    while(serialDataAvail(gpsHandler) > 0)
    		serialFlush(gpsHandler);    // Clean the input buffer

    serialPuts(gpsHandler, AT);    // Send the AT sendATCommand
    previous = millis();

    //Wait for the answer
    do{
        if(serialDataAvail(gpsHandler) > 0)
        {
            // if there are data in the UART input buffer, reads it and checks for the answer
            answer[i] = serialGetchar(gpsHandler);
		  		  //cout << answer[i];
            i++;
            // check if the desired answer  is in the response of the module
            if (strstr(answer, expected_answer) != NULL)
            {
               	flag = 1;
            }
        }
     } while((flag == 0) && ((millis() - previous) < timeout));

    return flag;
}


//AT+CGPSINF=4 <messageID>,<latitude>,<N/S>,<longitude>,<E/W>,<UTCtime>,<status>,<mode>
void CGps :: readGps()
{
  char answer[100];
  int i = 0;
  int8_t flag = 0;
  int8_t messageID;
  unsigned long previous;
  float flatitude, flongitude;
  float fdegrees, fminutes, UTCtime;
  char NS_indicator, EW_indicator, status;


  memset(answer, '\0', 100);    // Initialize the string

  //Request GPGLL
	sendATCommand("AT+CGPSINF=4\r\n", "\r\n", 2000);

  previous = millis();

  do {
    if(serialDataAvail(gpsHandler) > 0)
    {
      answer[i] = serialGetchar(gpsHandler);

      if(answer[i] == 'A' || answer[i] == 'V') {
        flag = 1;
      }
      i++;
    }

  } while((flag == 0) && ((millis() - previous) < 2000));

  while(serialDataAvail(gpsHandler) > 0) {
    serialFlush(gpsHandler);
  }

  //<messageID>,<latitude>,<N/S>,<longitude>,<E/W>,<UTCtime>,<status>
  sscanf(answer, "%i,%f,%c,%f,%c,%f,%c", &messageID, &flatitude, &NS_indicator, &flongitude, &EW_indicator, &UTCtime, &status);

  // cout << "Answer: " << answer << endl
  // << "flatitude: " << flatitude << endl
  // << "NS_indicator: " << NS_indicator << endl
  // << "flongitude: " << flongitude << endl
  // << "EW_indicator: " << EW_indicator << endl;

  if(status == 'A') // Data Valid
  {
    fdegrees = (int)flatitude / 100;
    //cout << "fdegrees: " << fdegrees << endl;
    fminutes = (flatitude - fdegrees * 100) / 60;
    //cout << "fminutes: " << fminutes << endl;
    mf_latitude = fdegrees + fminutes;


    if(NS_indicator == 'S') {
      mf_latitude = -mf_latitude;
    }

    fdegrees = (int)flongitude / 100;
    //cout << "fdegrees: " << fdegrees << endl;
    fminutes = (flongitude - fdegrees * 100) / 60;
    //cout << "fminutes: " << fminutes << endl;
    mf_longitude = fdegrees + fminutes;

    if(EW_indicator == 'W') {
      mf_longitude = -mf_longitude;
    }

    mi_UTCtime = (int)UTCtime;

    mb_gpsData = true;

    cout << setprecision(6) << fixed;
    cout << "LATITUDE:  " << mf_latitude << endl;
    cout << "LONGITUDE: " << mf_longitude << endl;
  }
  else //Data not valid - Read again
  {
    mb_gpsData = false;
    readGps();
    cout << "Read again..." << endl;
  }
}

bool CGps :: gpsDataStatus()
{
  return mb_gpsData;
}

float CGps :: getLatitude()
{
  return mf_latitude;
}

float CGps :: getLongitude()
{
  return mf_longitude;
}
