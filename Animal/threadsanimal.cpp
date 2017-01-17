#include "threadsanimal.h"

/***** MUTEX *****/
pthread_mutex_t *mutex_sendInfo;
pthread_mutex_t *mutex_GPSReady;
pthread_mutex_t *mutex_endProcessing;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfo;
pthread_cond_t *ts_endProcessing;
pthread_cond_t *ts_GPSReady;

/***** Objects *****/
CRFCom *m_rf;
//CGps *m_gps;
CAnimal *m_animal;

CThreadsAnimal::CThreadsAnimal()
{
  /***** MUTEX *****/
  mutex_sendInfo = new pthread_mutex_t();
  int m_sendInfo_status=pthread_mutex_init(mutex_sendInfo,NULL);
  mutex_GPSReady = new pthread_mutex_t();
  int mutex_GPSReady_status=pthread_mutex_init(mutex_GPSReady, NULL);
  mutex_endProcessing = new pthread_mutex_t();
  int mutex_endProcessing_status=pthread_mutex_init(mutex_endProcessing, NULL);

  /***** SIGNALS *****/
  ts_sendInfo = new pthread_cond_t();
  int ts_sendInfo_status=pthread_cond_init(ts_sendInfo, NULL);
  ts_endProcessing = new pthread_cond_t();
  int ts_endProcessing_status=pthread_cond_init(ts_endProcessing, NULL);
  ts_GPSReady = new pthread_cond_t();
  int ts_GPSReady_status=pthread_cond_init(ts_GPSReady, NULL);

  /***** QUEUES *****/
  mq_GPS = mq_open(MQGPS, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
  mq_rf = mq_open(MQRFCOM, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
  mq_batTemp = mq_open(MQRFCOM, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
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
  if (mq_batTemp == (mqd_t)-1) {
    perror("In mq_open()");
    exit(1);
  }

  mq_setattr(mq_rf, &attr, NULL); // set de attr
  attr.mq_msgsize=MQGPSLEN;
  mq_setattr(mq_GPS, &attr, NULL); // set de attr
  attr.mq_msgsize=MQBATTEMP;
  mq_setattr(mq_batTemp, &attr, NULL); // set de attr

  /***** OBJECT *****/
  m_rf = new CRFCom();
  m_animal = new CAnimal();

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
   pthread_detach(t_shock);
   pthread_detach(t_processinInfo);
}


void * CThreadsAnimal::pv_RFComSenderHandler(void *threadid)
{
  unsigned char requestedInfo[33];
  unsigned int sender;

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
    mq_rf = mq_open(MQRFCOM, O_RDWR);
    mq_receive(mq_rf, requestedInfo, MQRFCOMLEN, &sender);
    mq_close(mq_rf);

    //Save content to char message[33]

    //Send requested information
    m_rf->RFComSender(NULL,requestedInfo);
  }
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char message[33];
  CFieldMap fieldmap;

  while(1)
  {
    m_rf->RFComReceiver(message);  //Wait for a message

    //Check command
    switch (message[4]) { //Command Type
      //Reset Command "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
      case 'R':
      /******************
      ******************/
      break;

      //Request Information "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
      case 'I':
      break;

      //First Config "ID_Field,ID_Animal,N,ID_Animal,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
      case 'N':
      //Config Animal Info
      m_animal->
      //Config Animal GreenZone

      break;

      //New Config "ID_Field,ID_Animal,C,GreenZone_x1,GreenZone_x2,GreenZone_y1,GreenZone_y2"
      case 'C':
      //Config Animal GreenZone

      break;

      default:
      break;
    }
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
}

void * CThreadsAnimal :: pv_shockHandler(void *threadid)
{
  int8_t zone;
  CLeds leds;

  while(1)
  {
    //Wait for ts_endProcessing
    pthread_mutex_lock(&mutex_endProcessing);
    //Wait for ts_GPSReady
    pthread_cond_wait(&ts_endProcessing, &mutex_endProcessing);
    pthread_mutex_unlock(&mutex_endProcessing);

    //Check which zone is the Animal

    //Turn on the LED zone
    switch (zone) {
      case GREENZONE:
      leds.greenZone();
      break;

      case YELLOWZONE:
      leds.yellowZone();
      break;

      case REDZONE:
      leds.redZone();
      break;

      case OUTZONE:
      leds.outsideZone();
      break;

      default:
      break;
    }

  }
}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  CAdc adc;
  unsigned long previous;
  float ftemperature, fbatteryLevel;
  unsigned char message[6];
  cout << "thread t_batTemp" << endl;

  while(1)
  {
    previous = millis();
    //Wait for animal.timeout
    while((millis() - previous) < m_animal->getAnimalTimeout());

    memset(message, '\0', 6);

    //Read animal's temperature
    adc.readTemperature();
    ftemperature = adc.getTemperature();

    uint8_t inteiro = static_cast<int>(ftemperature);
    uint8_t decimal = (ftemperature - inteiro) * 100;

    message[0] = inteiro;
    message[1] = decimal;

    if(ftemperature < 38.5) { //Alert low temperature
      message[4] = 1; //Flag low temperature
    }
    else if(ftemperature > 39.5) { //Alert high temperature
      message[4] = 3; //Flag high temperature
    }
    else {
      message[4] = 2; //Flag normal temperature
    }

    //Read animal's battery level
    adc.readBatteryLevel();
    fbatteryLevel = adc.getBatteryLevel();

    uint8_t inteiro1 = static_cast<int>(fbatteryLevel);
    uint8_t decimal1 = (fbatteryLevel - inteiro1) * 100;

    message[2] = inteiro1;
    message[3] = decimal1;

    if(fbatteryLevel < MIN_BATTERY_LEVEL) { //Alert low battery level
      message[5] = 0; //Flag low battery
    }
    else {
      message[5] = 1; //Flag full battery
    }

    //Send info to mq_batTemp
    mq_batTemp = mq_open(MQBATTEMP, O_RDWR);
    mq_send(mq_batTemp, message, strlen(message)+1, 1);
    mq_close(mq_batTemp);
  }

}

void * CThreadsAnimal :: pv_processinInfoHandler(void *threadid)
{

  while (1)
  {
    pthread_mutex_lock(&mutex_GPSReady);
    //Wait for ts_GPSReady
    pthread_cond_wait(&ts_GPSReady, &mutex_GPSReady);
    pthread_mutex_unlock(&mutex_GPSReady);

    //Open mq_GPS

    //Calculate which zone is the animal

    //Set the animal zone and the timer of the zone


    pthread_mutex_lock(&mutex_endProcessing);
    //Trigger ts_endProcessing
    pthread_cond_signal(&ts_endProcessing);
    pthread_mutex_unlock(&mutex_endProcessing);
  }
}

void * CThreadsAnimal :: pv_gpsHandler(void *threadid)
{
  CGps gps;
  gps.initGps();
  char gpsCoordinates[50];

  cout << "thread t_gps" << endl;

  while(1)
  {
    previous = millis();
    //Wait for animal.timeout
    while((millis() - previous) < m_animal->getAnimalTimeout());

    //Read animal's coordinates
    gps.readGps();

    if(gps.gpsDataStatus()) // Send it to mq_GPS
    {
      memset(gpsCoordinates, '\0', 50);    // Initialize the string
      sprintf(gpsCoordinates, "%f;%f", gps.getLatitude(), gps.getLongitude());

      mq_GPS = mq_open(MQGPS, O_RDWR); //Send coordinates
      mq_send(mq_GPS, gpsCoordinates, strlen(gpsCoordinates)+1, 1);
      mq_close(mq_GPS);

      pthread_mutex_lock(&mutex_GPSReady);
      //Trigger ts_GPSReady
      pthread_cond_signal(&ts_GPSReady);
      pthread_mutex_unlock(&mutex_GPSReady);

    }
    else
    {

    }
  }

}
