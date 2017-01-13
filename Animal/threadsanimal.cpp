#include "threadsanimal.h"

/***** MUTEX *****/
pthread_mutex_t *mutex_sendInfo;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfo;
pthread_cond_t *ts_endProcessing;
pthread_cond_t *ts_GPSReady;

/***** Objects *****/
CRFCom *m_rf;
CGps *m_gps;

CThreadsAnimal::CThreadsAnimal()
{
  /***** MUTEX *****/
  mutex_sendInfo = new pthread_mutex_t();
  int m_sendInfo_status=pthread_mutex_init(mutex_sendInfo,NULL);

  /***** SIGNALS *****/
  ts_sendInfo = new pthread_cond_t();
  int ts_sendInfo_status=pthread_cond_init(ts_sendInfo, NULL);

  /***** QUEUES *****/
  mq_GPS = mq_open(MQGPS, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
  mq_rf = mq_open(MQRFCOM, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
  struct mq_attr attr;
  attr.mq_flags=O_NONBLOCK;
  attr.mq_maxmsg=2;
  attr.mq_msgsize=MQRFCOMLEN;
  attr.mq_curmsgs=0;
  if (mq_GPS == (mqd_t)-1) {
    perror("In mq_open()");
    exit(1);
  }
  if (mq_rf == (mqd_t)-1) {
    perror("In mq_open()");
    exit(1);
  }

  mq_setattr(mq_rf, &attr, NULL); // set de attr
  attr.mq_msgsize=MQGPSLEN;
  mq_setattr(mq_GPS, &attr, NULL); // set de attr

  /***** OBJECT *****/
  m_rf = new CRFCom();

  /***** THREADS *****/
  pthread_attr_t thread_attr=setAttr(60);
  int rfS_status = pthread_create(&t_RFComSender,&thread_attr,pv_RFComSenderHandler,(void*)m_rf);
  int rfR_status = pthread_create(&t_RFComReceiver, &thread_attr,
  pv_RFComReceiverHandler, (void*)m_rf);

  thread_attr=setAttr(90);
  int shock_status = pthread_create(&t_shock, &thread_attr,pv_shockHandler, NULL);

  thread_attr=setAttr(80);
  int GPS_status = pthread_create(&t_gps,&thread_attr,pv_gpsHandler,NULL);
  int batT_status = pthread_create(&t_batTemp, &thread_attr,pv_batTempHandler, NULL);

  thread_attr=setAttr(70);
  int pIn_status = pthread_create(&t_processinInfo, &thread_attr, pv_processinInfoHandler, NULL);

  if(rfS_status || rfR_status || shock_status || batT_status || pIn_status || GPS_status)
  {
    cout << "ERROR CREATING THREAD" << endl;
  }
}

CThreadsAnimal::~CThreadsAnimal()
{
  mq_close(mq_GPS);
  mq_close(mq_rf);
  mq_unlink(MQGPS);
  mq_unlink(MQRFCOM);
}

pthread_attr_t CThreadsAnimal::setAttr(int prio)
{
  pthread_attr_t thread_attr;
  int thread_policy;
  struct sched_param thread_param;
  pthread_attr_init (&thread_attr);
  pthread_attr_getschedpolicy(&thread_attr, &thread_policy);
  pthread_attr_getschedparam(&thread_attr, &thread_param);
  thread_param.sched_priority = prio;
  pthread_attr_setschedpolicy(&thread_attr,SCHED_RR);
  pthread_attr_setschedparam(&thread_attr,&thread_param);
  pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
  return thread_attr;
}

void CThreadsAnimal::run()
{
	 pthread_detach(t_RFComReceiver);
   pthread_detach(t_RFComSender);
   pthread_detach(t_gps);
   pthread_detach(t_batTemp);
}


void * CThreadsAnimal::pv_RFComSenderHandler(void *threadid)
{
  unsigned char message[33];

  while(1)
  {
    //Lock mutex_sendInfo
    pthread_mutex_lock(&mutex_sendInfo);
    //Wait for ts_sendInfo
    //while(something) //Verificar se mq_rf está completa
    pthread_cond_wait(&ts_sendInfo, &mutex_sendInfo);
    //Unlock mutex_sendInfo
    pthread_mutex_unlock(&mutex_sendInfo);

    //open mq_rf - information to send

    //Save content to char message[33]

    //Send requested information
    m_rf->RFComSender(NULL,message);
  }

	/*//while(1)
	//{
		pthread_cond_wait(ts_sendInfo, mutex_sendInfo);
		((CRFCom*)threadid)->RFComSender(NULL, NULL);
	//}*/
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char message[33];

  while(1)
  {
    m_rf->RFComReceiver(message);  //Wait for a message

    //Check command

    //Send command to mq_rf ?

    //It's a new configuration?
    //YES
    //Set the new configurations

    //NO
    pthread_mutex_lock(&mutex_sendInfo);
    //Trigger ts_sendInfo
    pthread_cond_signal(&ts_sendInfo);
    pthread_mutex_unlock(&mutex_sendInfo);
  }
	/*//while(1)
	//{
		((CRFCom*)threadid)->RFComReceiver();
		((CRFCom*)threadid)->RFComPrintTAddr();
	//}*/
}

void * CThreadsAnimal :: pv_shockHandler(void *threadid)
{
  //Wait for ts_endProcessing

  //Check which zone is the Animal

  //Turn on the LED zone

}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  CAdc adc;
  cout << "thread t_batTemp" << endl;

  while(1)
  {
    //Wait for animal.timeout
    //Read animal's temperature
    adc.readTemperature();

    //Read animal's battery level
    adc.readBatteryLevel();

    //Send info to mq_batTemp
    delay(3000);
  }

}

void * CThreadsAnimal :: pv_processinInfoHandler(void *threadid)
{
  //Wait for ts_GPSReady

  //Open mq_GPS

  //Calculate which zone is the animal

  //Set the animal zone and the timer of the zone

  //Trigger ts_endProcessing

  // char command[32]; //commando recebido por RF mq_rf
  //
  // /*************************
  // command[0-1] - ID_Field
  // command[2-3] - ID_Animal
  // command[4] - Command Type
  // *************************/
  //
  // switch (command[4]) {
  //   case 'R': //Reset "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
  //   /*****************************
  //   command[5] - Reset Temperature
  //   command[6] - Reset Battery
  //   command[7] - Reset GPS
  //   command[8] - Reset RF
  //   ****************************/
  //   if(command[5]){ //Reset Temperature
  //
  //   }
  //   else if(command[6]){ //Reset Battery
  //
  //   }
  //   else if(command[7]){ //Reset GPS
  //
  //   }
  //   else if(command[8]){ //Reset RF
  //
  //   }
  //   break;
  //
  //   case 'I':  //Field request animal info "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
  //   break;
  //
  //   case 'N': //First Configuration "ID_Field,ID_Animal,N,ID_Animal,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
  //   /*****************************
  //   command[5-6]   - ID_Animal
  //   command[7-12]  - GreenZone_x1
  //   command[13-18] - GreenZone_x2
  //   command[19-24] - GreenZone_y1
  //   command[25-29] - GreenZone_y2
  //   ****************************/
  //
  //   SSquare greenZone;
  //   greenZone.x1 = 2.0;
  //   break;
  //
  //   case 'C': //New Configuration "ID_Field,ID_Animal,C,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
  //   break;
  //
  //   default:
  //   break;
  // }

}

void * CThreadsAnimal :: pv_gpsHandler(void *threadid)
{
  CGps gps;
  gps.initGps();
  char gpsCoordinates[50];

  cout << "thread t_gps" << endl;

  while(1)
  {
    delay(2000);
    //Wait for animal.timeout
    //Read animal's coordinates
    gps.readGps();

    if(gps.gpsDataStatus()) // Send it to mq_GPS
    {
      memset(gpsCoordinates, '\0', 50);    // Initialize the string
      sprintf(gpsCoordinates, "%f;%f", gps.getLatitude(), gps.getLongitude());

      mq_GPS = mq_open(MQGPS, O_RDWR); //Send coordinates
      mq_send(mq_GPS, gpsCoordinates, strlen(gpsCoordinates)+1, 1);
      mq_close(mq_GPS);

      //Trigger ts_GPSReady

    }
    else
    {

    }
  }

}
