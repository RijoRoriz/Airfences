#include "threadsanimal.h"

/***** MUTEX *****/
pthread_mutex_t *mutex_sendInfo;
pthread_mutex_t *mutex_readInfo;
pthread_mutex_t *mutex_GPSReady;
pthread_mutex_t *mutex_endProcessing;
pthread_mutex_t *mutex_readGPS;
pthread_mutex_t *mutex_readBatTemp;
pthread_mutex_t *mutex_animalZone;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfo;
pthread_cond_t *ts_readInfo;

pthread_cond_t *ts_endProcessing;
pthread_cond_t *ts_GPSReady;
pthread_cond_t *ts_readGPS;
pthread_cond_t *ts_readBatTemp;


/***** QUEUES *****/
mqd_t mq_GPS;
mqd_t mq_rf;
mqd_t mq_batTemp;

/***** Objects *****/
CRFCom *p_rf;
//CGps *m_gps;
CAdc *p_adc;
CAnimal *p_animal;
CFieldMap *p_fieldMap

CThreadsAnimal::CThreadsAnimal()
{
  /***** MUTEX *****/
  mutex_sendInfo = new pthread_mutex_t();
  int m_sendInfo_status=pthread_mutex_init(mutex_sendInfo,NULL);
  mutex_readInfo = new pthread_mutex_t();
  int mutex_readInfo_status=pthread_mutex_init(mutex_readInfo,NULL);
  mutex_GPSReady = new pthread_mutex_t();
  int mutex_GPSReady_status=pthread_mutex_init(mutex_GPSReady, NULL);
  mutex_endProcessing = new pthread_mutex_t();
  int mutex_endProcessing_status=pthread_mutex_init(mutex_endProcessing, NULL);
  mutex_readGPS = new pthread_mutex_t();
  int mutex_readGPS_status=pthread_mutex_init(mutex_readGPS, NULL);
  mutex_readBatTemp = new pthread_mutex_t();
  int mutex_readBatTemp_status=pthread_mutex_init(mutex_readBatTemp, NULL);
  mutex_animalZone = new pthread_mutex_t();
  int mutex_animalZone_status=pthread_mutex_init(mutex_animalZone, NULL);

  /***** SIGNALS *****/
  ts_sendInfo = new pthread_cond_t();
  int ts_sendInfo_status=pthread_cond_init(ts_sendInfo, NULL);
  ts_readInfo = new pthread_cond_t();
  int ts_readInfo_status=pthread_cond_init(ts_readInfo, NULL);
  ts_endProcessing = new pthread_cond_t();
  int ts_endProcessing_status=pthread_cond_init(ts_endProcessing, NULL);
  ts_GPSReady = new pthread_cond_t();
  int ts_GPSReady_status=pthread_cond_init(ts_GPSReady, NULL);
  ts_readGPS = new pthread_cond_t();
  int ts_readGPS_status=pthread_cond_init(ts_readGPS, NULL);
  ts_readBatTemp = new pthread_cond_t();
  int ts_readBatTemp_status=pthread_cond_init(ts_readBatTemp, NULL);

  /***** QUEUES *****/
  mq_GPS = mq_open(MQGPS, O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);
  mq_rf = mq_open(MQRFCOM, O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);
  mq_batTemp = mq_open(MQBATTEMP, O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);
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
  attr.mq_msgsize=MQBATTEMPLEN;
  mq_setattr(mq_batTemp, &attr, NULL); // set de attr

  /***** OBJECT *****/
  p_rf = new CRFCom();
  p_animal = new CAnimal();
  p_fieldMap = new CFieldMap();
  p_adc = new CAdc();

  p_animal->setAnimalTimeout(GREENZONE);

  /***** THREADS *****/
  pthread_attr_t thread_attr=setAttr(60);
  int rfS_status = pthread_create(&t_RFComSender,&thread_attr,pv_RFComSenderHandler,(void*)p_rf);
  int rfR_status = pthread_create(&t_RFComReceiver, &thread_attr,
  pv_RFComReceiverHandler, (void*)p_rf);

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
  pv_initTimer();
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

   pthread_cond_signal(ts_readInfo);
}

void CThreadsAnimal::pv_initTimer()
{
  timer_t timerid;
  struct sigevent sev;
  struct itimerspec its;
  long long freq_nanosecs;
  sigset_t mask;
  struct sigaction sa;

  /* Establish handler for timer signal */
  //printf("Establishing handler for signal %d\n", SIGALRM);
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = pv_handleTimer;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, NULL) == -1)
    errExit("sigaction");

  /* Create the timer */
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM; //	14	/* alarm clock */
  sev.sigev_value.sival_ptr = &timerid;
  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    errExit("timer_create");

  //printf("timer ID is 0x%lx\n", (long) timerid);

  /* Start the timer */
  its.it_value.tv_sec = TIME_RED_ZONE; //10 seconds
  its.it_interval.tv_sec = TIME_RED_ZONE; //10 seconds

  if (timer_settime(timerid, 0, &its, NULL) == -1)
    errExit("timer_settime");

}

void CThreadsAnimal::pv_handleTimer(int sig, siginfo_t *si, void *uc)
{
  timer_t timerid;
  struct itimerspec its;
  int timeout = p_animal->getAnimalTimeout();
  timerid = si->si_timerid;

  //cout << "Timeout" << endl;

  pthread_mutex_lock(mutex_readBatTemp);
  pthread_cond_signal(ts_readBatTemp);
  pthread_mutex_unlock(mutex_readBatTemp);

  pthread_mutex_lock(mutex_readGPS);
  pthread_cond_signal(ts_readGPS);
  pthread_mutex_unlock(mutex_readGPS);

  /* Update the timer */
  its.it_value.tv_sec = timeout;
  its.it_interval.tv_sec = timeout;

  //Update timer values
  if (timer_settime(timerid, 0, &its, NULL) == -1)
    errExit("timer_settime");
}


void * CThreadsAnimal::pv_RFComSenderHandler(void *threadid)
{
  char crequestedInfo[33];
  unsigned char requestedInfo[33];

  unsigned int sender;

  cout << "thread pv_RFComSenderHandler" << endl;

  while(1)
  {
    memset(crequestedInfo, '\0', 33);
    memset(requestedInfo, '3', 33);
    //Wait for ts_sendInfo
    //while(something) //Verificar se mq_rf está completa
    pthread_cond_wait(ts_sendInfo, mutex_sendInfo);
    cout << "Ready to send message" << endl;

    // //open mq_rf - information to send
    // mq_rf = mq_open(MQRFCOM, O_RDWR);
    // mq_receive(mq_rf, crequestedInfo, MQRFCOMLEN, &sender);
    // mq_close(mq_rf);

    // //Save content to char message[33]
    // for(int i=0; i < 33; i++)
    // {
    //   requestedInfo[i] = (unsigned char) crequestedInfo[i];
    // }
    //
    //Send requested information
    p_rf->RFComSender(NULL, requestedInfo);
    pthread_mutex_lock(mutex_readInfo);
    pthread_cond_signal(ts_readInfo);
    pthread_mutex_unlock(mutex_readInfo);
  }
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char message[33];
  cout << "thread pv_RFComReceiverHandler" << endl;

  while(1)
  {
    pthread_cond_wait(ts_readInfo, mutex_readInfo);

    cout << "Waiting message" << endl;
    p_rf->RFComReceiver(message);  //Wait for a message

    cout << "MESSAGE RECEIVEID: ";
    for(int i=0; i<33; i++)
    {
      printf("%x", message[i] );
    }
    cout << endl;

    //Check command
    switch (message[4]) { //Command Type
      //Reset Command "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
      case 'R':
      if(message[5] == 1) { //Reset Temperature

      }
      if(message[6] == 1) { //Reset Battery

      }
      if(message[7] == 1) { //Reset GPS

      }
      if(message[8] == 1) { //Reset RF

      }
      break;

      //Request Information "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
      case 'I':
      //Send message to mq_rf?

      pthread_mutex_lock(mutex_sendInfo);
      //Trigger ts_sendInfo
      pthread_cond_signal(ts_sendInfo);
      pthread_mutex_unlock(mutex_sendInfo);
      break;

      //First Config or New Conf
      case 'N':
      case 'C':
      //Config Animal Info
      p_animal->setAnimalConf(message);
      //Config Animal GreenZone
      p_fieldMap->m_configureMap(p_animal->getAnimalGreenZone());
      break;

      default:
      break;
    }

  }
}

void * CThreadsAnimal :: pv_shockHandler(void *threadid)
{
  int iAnimalZone;
  CLeds leds;
  cout << "thread pv_shockHandler" << endl;

  while(1)
  {
    //Wait for ts_endProcessing
    pthread_cond_wait(ts_endProcessing, mutex_endProcessing);

    //Check which zone is the Animal
    pthread_mutex_lock(mutex_animalZone);
    iAnimalZone = p_animal->getAnimalZone();
    pthread_mutex_unlock(mutex_animalZone);

    //Turn on the LED zone
    leds.m_turnON_LedZone(iAnimalZone);

  }
}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  float ftemperature, fbatteryLevel;
  unsigned char message[6];
  char cmessage[6];
  cout << "thread pv_batTempHandler" << endl;

  while(1)
  {
    //Wait for animal.timeout
    pthread_cond_wait(ts_readBatTemp, mutex_readBatTemp);
    cout << "BAT TEMP START READING" << endl;
    // memset(message, '\0', 6);
    // memset(cmessage, '\0', 6);
    //
    // //Read animal's temperature
    // adc.readTemperature();
    // ftemperature = adc.getTemperature();
    //
    // uint8_t inteiro = static_cast<int>(ftemperature);
    // uint8_t decimal = (ftemperature - inteiro) * 100;
    //
    // message[0] = inteiro;
    // message[1] = decimal;
    //
    // if(ftemperature < 38.5) { //Alert low temperature
    //   message[4] = 1; //Flag low temperature
    // }
    // else if(ftemperature > 39.5) { //Alert high temperature
    //   message[4] = 3; //Flag high temperature
    // }
    // else {
    //   message[4] = 2; //Flag normal temperature
    // }
    //
    // //Read animal's battery level
    // adc.readBatteryLevel();
    // fbatteryLevel = adc.getBatteryLevel();
    //
    // uint8_t inteiro1 = static_cast<int>(fbatteryLevel);
    // uint8_t decimal1 = (fbatteryLevel - inteiro1) * 100;
    //
    // message[2] = inteiro1;
    // message[3] = decimal1;
    //
    // if(fbatteryLevel < MIN_BATTERY_LEVEL) { //Alert low battery level
    //   message[5] = 0; //Flag low battery
    // }
    // else {
    //   message[5] = 1; //Flag full battery
    // }
    //
    // for(int i=0; i < 6; i++)
    // {
    //   cmessage[i] = (char)message[i];
    // }
    //
    // //Send info to mq_batTemp
    // mq_batTemp = mq_open(MQBATTEMP, O_RDWR);
    // mq_send(mq_batTemp, cmessage, strlen(cmessage)+1, 1);
    // mq_close(mq_batTemp);
  }

}

void * CThreadsAnimal :: pv_processinInfoHandler(void *threadid)
{
  int iAnimalZone;
  cout << "thread pv_processinInfoHandler" << endl;

  while (1)
  {
    //Wait for ts_GPSReady
    pthread_cond_wait(ts_GPSReady, mutex_GPSReady);

    //Open mq_GPS

    //Calculate which zone is the animal
    cout << "Calculating Animal Zone" << endl;
    //iAnimalZone = p_fieldMap->mi_checkAnimalZone(lat1, lat2);

    //Set the animal zone and the timer of the zone
    pthread_mutex_lock(mutex_animalZone);
    p_animal->setAnimalZone(iAnimalZone);
    p_animal->setAnimalTimeout(iAnimalZone);
    pthread_mutex_unlock(mutex_animalZone);


    pthread_mutex_lock(mutex_endProcessing);
    //Trigger ts_endProcessing
    pthread_cond_signal(ts_endProcessing);
    pthread_mutex_unlock(mutex_endProcessing);
  }
}

void * CThreadsAnimal :: pv_gpsHandler(void *threadid)
{
  //CGps gps;
  //gps.initGps();
  char gpsCoordinates[50];
  unsigned long previous;

  cout << "thread pv_gpsHandler" << endl;

  while(1)
  {
    //previous = millis();
    //Wait for animal.timeout
    //while((millis() - previous) < p_animal->getAnimalTimeout());
    pthread_cond_wait(ts_readGPS, mutex_readGPS);
    cout << "GPS START READING" << endl;
    //Read animal's coordinates
    // gps.readGps();
    //
    // if(gps.gpsDataStatus()) // Send it to mq_GPS
    // {
    //   memset(gpsCoordinates, '\0', 50);    // Initialize the string
    //   sprintf(gpsCoordinates, "%f;%f", gps.getLatitude(), gps.getLongitude());
    //
    //   mq_GPS = mq_open(MQGPS, O_RDWR); //Send coordinates
    //   mq_send(mq_GPS, gpsCoordinates, strlen(gpsCoordinates)+1, 1);
    //   mq_close(mq_GPS);
    //
    //   pthread_mutex_lock(mutex_GPSReady);
    //   //Trigger ts_GPSReady
    //   pthread_cond_signal(ts_GPSReady);
    //   pthread_mutex_unlock(mutex_GPSReady);
    //
    // }
    // else
    // {
    //
    // }
  }

}
