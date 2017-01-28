#include "threadsanimal.h"

/***** MUTEX *****/
pthread_mutex_t *mutex_sendInfo;
pthread_mutex_t *mutex_readInfo;
pthread_mutex_t *mutex_GPSReady;
pthread_mutex_t *mutex_endProcessing;
pthread_mutex_t *mutex_readGPS;
pthread_mutex_t *mutex_readBatTemp;
pthread_mutex_t *mutex_queue_rf;

pthread_mutex_t *mutex_animalZone;
pthread_mutex_t *mutex_requestInfo;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfo;
pthread_cond_t *ts_readInfo;
pthread_cond_t *ts_requestInfo;

pthread_cond_t *ts_endProcessing;
pthread_cond_t *ts_GPSReady;
pthread_cond_t *ts_readGPS;
pthread_cond_t *ts_readBatTemp;


/***** QUEUES *****/
mqd_t mq_GPS;
mqd_t mq_rfReceiver;
mqd_t mq_rfSender;
mqd_t mq_batTemp;

/***** Objects *****/
CRFCom *p_rf;
CGps *p_gps;
CAdc *p_adc;
CAnimal *p_animal;
CFieldMap *p_fieldMap;

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
  /***********************************************************************/
  mutex_queue_rf = new pthread_mutex_t();
  int mutex_queue_rf_status=pthread_mutex_init(mutex_queue_rf, NULL);
  mutex_requestInfo = new pthread_mutex_t();
  int mutex_requestInfo_status=pthread_mutex_init(mutex_requestInfo, NULL);
  /***********************************************************************/

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
  /***********************************************************************/
  ts_requestInfo = new pthread_cond_t();
  int ts_requestInfo_status=pthread_cond_init(ts_requestInfo, NULL);
  /***********************************************************************/

  /***** QUEUES *****/
  mq_close(mq_GPS);
  mq_GPS = mq_open(MQGPS, O_RDWR | O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);

  if (mq_GPS == (mqd_t)-1) {
    perror("CThreadsAnimal::CThreadsAnimal In mq_open()");
    //exit(1);
  }
  mq_close(mq_rfReceiver);
  mq_rfReceiver = mq_open(MQRFCOM, O_RDWR | O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);

  if (mq_rfReceiver == (mqd_t)-1) {
    perror("CThreadsAnimal::CThreadsAnimal In mq_open()");
    //exit(1);
  }

  mq_close(mq_batTemp);
  mq_batTemp = mq_open(MQBATTEMP, O_RDWR | O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);

  if (mq_batTemp == (mqd_t)-1) {
    perror("CThreadsAnimal::CThreadsAnimal In mq_open()");
    //exit(1);
  }

  mq_close(mq_rfSender);
  mq_rfSender = mq_open(MQRFSENDER, O_RDWR | O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);

  if (mq_rfSender == (mqd_t)-1) {
    perror("CThreadsAnimal::CThreadsAnimal In mq_open()");
    //exit(1);
  }

  /***** OBJECT *****/
  p_rf = new CRFCom();
  p_animal = new CAnimal();
  p_fieldMap = new CFieldMap();
  p_adc = new CAdc();
  p_gps = new CGps();

  p_animal->m_setAnimalTimeout(REDZONE);
  //p_gps->initGps();
  p_fieldMap->m_configureMap(p_animal->mssq_getAnimalFenceLimits());

  /***** THREADS *****/
  pthread_attr_t thread_attr=setAttr(60);
  int rfS_status = pthread_create(&t_RFComSender,&thread_attr,pv_RFComSenderHandler, NULL);
  int rfR_status = pthread_create(&t_RFComReceiver, &thread_attr,
  pv_RFComReceiverHandler, NULL);

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
  mq_close(mq_batTemp);
  mq_close(mq_rfReceiver);
  mq_close(mq_rfSender);
  // mq_unlink(MQGPS);
  // mq_unlink(MQRFCOM);
  // mq_unlink(MQRFSENDER);
  // if(mq_unlink(MQBATTEMP) == -1)
  //   perror("Removing queue error");
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
  pthread_detach(t_gps);
  pthread_detach(t_batTemp);
  pthread_detach(t_shock);
  pthread_detach(t_processinInfo);
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);

  pthread_mutex_lock(mutex_readInfo);
  pthread_cond_signal(ts_readInfo);
  pthread_mutex_unlock(mutex_readInfo);
}

void CThreadsAnimal::pv_initTimer()
{
  timer_t mt_timerid;
  struct sigevent sev;
  struct sigaction sa;
  struct itimerspec its;

  /* Establish handler for timer signal */
  //printf("Establishing handler for signal %d\n", SIGRTMAX);
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = pv_TimerHandler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGRTMAX, &sa, NULL) == -1)
    perror("CThreadsAnimal::pv_initTimer In sigaction");

  /* Create the timer */
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMAX;
  sev.sigev_value.sival_ptr = mt_timerid;
  if (timer_create(CLOCK_REALTIME, &sev, &mt_timerid) == -1)
    perror("CThreadsAnimal::pv_initTimer In timer_create");

  //printf("timer ID is 0x%lx\n", (long) timerid);

  /* Start the timer */
  its.it_value.tv_sec = 5; //10 seconds
  its.it_interval.tv_sec = 5; //10 seconds

  if (timer_settime(mt_timerid, 0, &its, NULL) == -1)
    perror("CThreadsAnimal::pv_initTimer In timer_settime");

}

void CThreadsAnimal::pv_updateTimer()
{
  // struct itimerspec its;
  //
  // int timeout = p_animal->mi_getAnimalTimeout();
  //
  // cout << "Timeout: " << timeout << endl;
  //
  // /* Update the timer */
  // its.it_value.tv_sec = timeout;
  // its.it_interval.tv_sec = timeout;
  //
  // //Update timer values
  // if (timer_settime(mt_timerid, 0, &its, NULL) == -1)
  //   perror("CThreadsAnimal::pv_updateTimer In timer_settime");
}

void CThreadsAnimal :: pv_TimerHandler(int sig, siginfo_t *si, void *uc)
{
  // CAnimal *animal;

  if(sig == SIGRTMAX) {
    cout << "TIMEOUT" << endl;
    pthread_mutex_lock(mutex_readBatTemp);
    pthread_cond_signal(ts_readBatTemp);
    pthread_mutex_unlock(mutex_readBatTemp);

    pthread_mutex_lock(mutex_readGPS);
    pthread_cond_signal(ts_readGPS);
    pthread_mutex_unlock(mutex_readGPS);

    // animal = static_cast<CAnimal*>(si->si_value.sival_ptr);
    //animal->pv_updateTimer();
  }
}

void * CThreadsAnimal::pv_RFComSenderHandler(void *threadid)
{
  char cCommand[MAX_MSG_LEN];
  char cAnimalInfo[MAX_MSG_LEN];
  unsigned char requestedInfo[33];
  unsigned int sender;
  int msgsz;
  unsigned char addrT[4] = {0xFF, 0xFF, 0x00, 0x01};
  struct mq_attr attr;

  #if DEBUG
  cout << "thread pv_RFComSenderHandler" << endl;
  #endif

  while(1)
  {
    memset(cCommand, '\0', MAX_MSG_LEN);
    memset(requestedInfo, '\0', 33);
    memset(cAnimalInfo, '\0', MAX_MSG_LEN);
    //Wait for ts_sendInfo
    pthread_mutex_lock(mutex_sendInfo);
    pthread_cond_wait(ts_sendInfo, mutex_sendInfo);
    pthread_mutex_unlock(mutex_sendInfo);

    cout << "Ready to send message" << endl;

    //open mq_rfReceiver - information to send
    mq_rfReceiver = mq_open(MQRFCOM, O_RDWR);
    if (mq_rfReceiver == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_RFComSenderHandler In mq_open()");
      //exit(1);
    }

    msgsz = mq_receive(mq_rfReceiver, cCommand, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
      perror("CThreadsAnimal::pv_RFComSenderHandler In mq_receive()");
      //exit(1);
    }
    mq_close(mq_rfReceiver);

    #if DEBUG
    cout << "mq_receive(mq_rfReceiver): ";
    for(int i=0; i < MQRFCOMLEN; i++) {
      printf("%X", cCommand[i]);
    }
    cout << endl;
    #endif

    //Open mq_rfSender
    mq_close(mq_rfSender);
    // pthread_mutex_lock(mutex_queue_rf);
    mq_rfSender = mq_open(MQRFSENDER, O_RDONLY); //Read coordinates
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_RFComSenderHandler In mq_open()");
      //exit(1);
    }
    // pthread_mutex_lock(mutex_queue_rf);
    msgsz = mq_receive(mq_rfSender, cAnimalInfo, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
      perror("CThreadsAnimal::pv_RFComSenderHandler In mq_receive()");
      //exit(1);
    }
    // pthread_mutex_unlock(mutex_queue_rf);
    mq_close(mq_rfSender);

    cout << "mq_receive(mq_rfSender): ";
    for(int i=0; i < 33; i++) {
      printf("%X", cAnimalInfo[i]);
    }
    cout << endl;

    // if(msgsz > 0) {
    //   //Check what to send
    //
    //
    //
    //
    //   switch (cCommand[4]) {
    //     case 'R':
    //     case 'N': //Send message received to field to know that the message was delivered
    //     case 'C':
    //     for(int i=0; i < MQRFCOMLEN; i++) {
    //       requestedInfo[i] = static_cast<unsigned char>(cCommand[i]);
    //     }
    //     break;
    //
    //     case 'I': // Request Info
    //     if(cCommand[5] == 1) { //Request Temperature
    //       memcpy(&requestedInfo[4], &cAnimalInfo[4],4);
    //     }
    //     if(cCommand[6] == 1) { //Request Battery
    //       memcpy(&requestedInfo[8], &cAnimalInfo[8],4);
    //     }
    //     if(cCommand[7] == 1) { //Request GPS
    //       memcpy(&requestedInfo[12], &cAnimalInfo[12], 1);
    //       memcpy(&requestedInfo[14], &cAnimalInfo[14], 4);
    //       memcpy(&requestedInfo[18], &cAnimalInfo[18], 4);
    //
    //     }
    //     if(cCommand[8] == 1) { //Request RF
    //
    //     }
    //     break;
    //
    //     default:
    //     break;
    //   }
    //
    //
    // }
    // else {
    //
    // }
    // //Save content to char message[33]
    // for(int i=0; i < 33; i++)
    // {
    //   requestedInfo[i] = (unsigned char) cCommand[i];
    // }
    //
    //Send requested information
    p_rf->RFComSender(addrT, requestedInfo);
    pthread_mutex_lock(mutex_readInfo);
    pthread_cond_signal(ts_readInfo);
    pthread_mutex_unlock(mutex_readInfo);
  }
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char message[MQRFCOMLEN];
  char msg2queue[MQRFCOMLEN];
  unsigned int msgqRF_prio = 1;
  unsigned char addrR[4] = {0xAA, 0xAA, 0x00, 0x01};

  #if DEBUG
  cout << "thread pv_RFComReceiverHandler" << endl;
  #endif

  p_rf->RFComSetAddR(addrR);

  while(1)
  {
    pthread_cond_wait(ts_readInfo, mutex_readInfo);
    memset(message, '\0', MQRFCOMLEN);
    memset(msg2queue, '\0', MQRFCOMLEN);

    cout << "Waiting message" << endl;
    p_rf->RFComReceiver(message);  //Wait for a message

    cout << "MESSAGE RECEIVEID: ";
    for(int i=0; i < MQRFCOMLEN; i++)
    {
      printf("%X", message[i] );
    }
    cout << endl;

    //Check command
    switch (message[4]) { //Command Type
      //Reset Command "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
      case 'R':
      cout << "RESET COMMAND" << endl;
      if(message[5] == 1) { //Reset Temperature
        p_adc->readTemperature();
      }
      if(message[6] == 1) { //Reset Battery
        p_adc->readBatteryLevel();
      }
      if(message[7] == 1) { //Reset GPS
        p_gps->readGps();
      }
      if(message[8] == 1) { //Reset RF
        //Restart RF
      }
      break;

      //Request Information "ID_Field,ID_Animal,I,Temperature,Battery,GPS,RF,State"
      case 'I':
      cout << "REQUEST COMMAND" << endl;
      break;

      //First Config or New Conf
      case 'N':
      case 'C':
      cout << "FIRST OR NEW CONF COMMAND" << endl;
      //Config Animal Info
      p_animal->m_setAnimalConf(message);
      //Config Animal GreenZone
      p_fieldMap->m_configureMap(p_animal->mssq_getAnimalFenceLimits());
      break;

      default:
      break;
    }

    for(int i=0; i < MQRFCOMLEN; i++) {
      msg2queue[i]=static_cast<char>(message[i]);
    }

    // mq_close(mq_rfReceiver);
    mq_rfReceiver = mq_open(MQRFCOM, O_RDWR);
    if (mq_rfReceiver == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_RFComReceiverHandler In mq_open()");
      //exit(1);
    }

    mq_rfReceiver = mq_send(mq_rfReceiver, msg2queue, MQRFCOMLEN, msgqRF_prio);
    if (mq_rfReceiver == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_RFComReceiverHandler In mq_send()");
      //exit(1);
    }
    mq_close(mq_rfReceiver);

    #if DEBUG
    cout << "mq_send(mq_rfReceiver): ";
    for(int i=0; i < MQRFCOMLEN; i++) {
      printf("%X", msg2queue[i]);
    }
    cout << endl;
    #endif

    cout << "Signal ts_sendInfo" << endl;
    pthread_mutex_lock(mutex_sendInfo);
    //Trigger ts_sendInfo
    pthread_cond_signal(ts_sendInfo);
    pthread_mutex_unlock(mutex_sendInfo);

  }
}

void * CThreadsAnimal :: pv_shockHandler(void *threadid)
{
  int iAnimalZone;
  CLeds leds;

  #if DEBUG
  cout << "thread pv_shockHandler" << endl;
  #endif

  while(1)
  {
    //Wait for ts_endProcessing
    pthread_mutex_lock(mutex_endProcessing);
    pthread_cond_wait(ts_endProcessing, mutex_endProcessing);
    pthread_mutex_unlock(mutex_endProcessing);
    cout << "Ready to trigger shock" << endl;

    //Check which zone is the Animal
    pthread_mutex_lock(mutex_animalZone);
    iAnimalZone = p_animal->mi_getAnimalZone();
    pthread_mutex_unlock(mutex_animalZone);
    cout << "Animal Zone: " << iAnimalZone << endl;
    //Turn on the LED zone
    leds.m_turnON_LedZone(iAnimalZone);

  }
}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  float ftemperature, fbatteryLevel;
  char msg_batTemp[20];
  char buffer[MAX_MSG_LEN];
  struct mq_attr attr;
  unsigned int sender;
  int msgsz;

  #if DEBUG
  cout << "thread pv_batTempHandler" << endl;
  #endif

  while(1)
  {
    //Wait for animal.timeout
    pthread_mutex_lock(mutex_readBatTemp);
    pthread_cond_wait(ts_readBatTemp, mutex_readBatTemp);
    pthread_mutex_unlock(mutex_readBatTemp);

    cout << "BAT TEMP START READING" << endl;
    memset(msg_batTemp, '\0', 20);

    //Read animal's temperature
    p_adc->readTemperature();
    ftemperature = p_adc->getTemperature();
    cout << "TEMP: " << ftemperature << endl;

    memcpy(&msg_batTemp[0], &ftemperature, 4);

    // if(ftemperature < 38.5) { //Alert low temperature
    //   msg_batTemp[4] = 1; //Flag low temperature
    // }
    // else if(ftemperature > 39.5) { //Alert high temperature
    //   msg_batTemp[4] = 3; //Flag high temperature
    // }
    // else {
    //   msg_batTemp[4] = 2; //Flag normal temperature
    // }

    //Read animal's battery level
    p_adc->readBatteryLevel();
    fbatteryLevel = p_adc->getBatteryLevel();
    cout << "BAT: " << fbatteryLevel << endl;

    memcpy(&msg_batTemp[4], &fbatteryLevel, 4);

    // if(fbatteryLevel < MIN_BATTERY_LEVEL) { //Alert low battery level
    //   msg_batTemp[5] = 0; //Flag low battery
    // }
    // else {
    //   msg_batTemp[5] = 1; //Flag full battery
    // }

    //Send info to mq_batTemp
    mq_batTemp = mq_open(MQBATTEMP, O_RDWR);


    // mq_getattr(mq_batTemp, &attr);
    // cout << "Before:" << endl;
    // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    // printf("Maximum message size: %ld\n", attr.mq_msgsize);
    // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);


    if (mq_batTemp == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_batTempHandler In mq_open()");
      //exit(1);
    }

    // if(attr.mq_curmsgs > 2) {
    //   cout << "maior q dois"  << endl;
    //   msgsz = mq_receive(mq_batTemp, buffer, MAX_MSG_LEN, &sender);
    //   if (msgsz == -1) {
    //     perror("CThreadsAnimal::pv_batTempHandler In mq_receive()");
    //     //exit(1);
    //   }
    //   // mq_close(mq_batTemp);
    //
    // }

    mq_batTemp = mq_send(mq_batTemp, msg_batTemp, 20, 1);
    if (mq_batTemp == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_batTempHandler In mq_send()");
      //exit(1);
    }
    mq_close(mq_batTemp);

    // mq_getattr(mq_batTemp, &attr);
    // cout << "After: " << endl;
    // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    // printf("Maximum message size: %ld\n", attr.mq_msgsize);
    // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);
    //
    //
    // //
    // cout << "mq_batTemp - pv_batTempHandler: ";
    // for(int i=0; i < strlen(msg_batTemp); i++) {
    //   printf("%X", msg_batTemp[i]);
    // }
    // cout << endl;
  }

}

void * CThreadsAnimal :: pv_processinInfoHandler(void *threadid)
{
  int iAnimalZone;
  char cAnimalCoordinates[MAX_MSG_LEN];
  char cbatTempInfo[MAX_MSG_LEN];
  char buffer[MAX_MSG_LEN];
  char cAnimalInfo[33];
  unsigned int sender;
  unsigned int msgqGPS_prio = 1;
  int msgsz;
  float latitude, longitude;
  struct mq_attr attr;

  #if DEBUG
  cout << "thread pv_processinInfoHandler" << endl;
  #endif

  while (1)
  {
    //Wait for ts_GPSReady
    pthread_mutex_lock(mutex_GPSReady);
    pthread_cond_wait(ts_GPSReady, mutex_GPSReady);
    pthread_mutex_unlock(mutex_GPSReady);
    cout << "Caught ts_GPSReady" << endl;
    //cout << "Calculating Animal Zone" << endl;
    memset(cAnimalCoordinates, '\0', MAX_MSG_LEN);
    memset(cbatTempInfo, '\0', MAX_MSG_LEN);
    memset(cAnimalInfo, '\0', 33);

    // pthread_mutex_lock(mutex_queue_rf);
    //Open mq_GPS
    //mq_close(mq_GPS);
    mq_GPS = mq_open(MQGPS, O_RDWR); //Read coordinates
    if (mq_GPS == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_open()");
      //exit(1);
    }

    msgsz = mq_receive(mq_GPS, cAnimalCoordinates, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
  	    perror("CThreadsAnimal::pv_processinInfoHandler In mq_receive()");
  	    //exit(1);
  	}

    // mq_getattr(mq_GPS, &attr);
    // cout << "thread pv_processinInfoHandler" << endl;
    // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    // printf("Maximum message size: %ld\n", attr.mq_msgsize);
    // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);

    mq_close(mq_GPS);

    // cout << "Bytes: " << msgsz << " mq_GPS: " << cAnimalCoordinates << endl;

    sscanf(cAnimalCoordinates, "%f;%f", &latitude, &longitude);
    //Calculate which zone is the animal
    cout << "Calculating Animal Zone" << endl;
    // cout << setprecision(6) << fixed
    // << "Latitude: " << latitude << endl
    // << "Longitude: " << longitude << endl;

    pthread_mutex_lock(mutex_animalZone);
    //Check and Return AnimalZone
    iAnimalZone = p_fieldMap->mi_checkAnimalZone(latitude, longitude);
    cout << "PI_AnimalZone: " << iAnimalZone << endl;
    //Set the animal zone and the timer of the zone
    p_animal->m_setAnimalZone(iAnimalZone);
    p_animal->m_setAnimalTimeout(iAnimalZone);
    pthread_mutex_unlock(mutex_animalZone);

    //GPS info
    // memcpy(&cAnimalInfo[12], &iAnimalZone, 1 //VERIFICAR
    memcpy(&cAnimalInfo[14], &latitude, 4);
    memcpy(&cAnimalInfo[18], &longitude, 4);

    mq_batTemp = mq_open(MQBATTEMP, O_RDWR);
    if (mq_batTemp == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_open()");
      //exit(1);
    }
    msgsz = mq_receive(mq_batTemp, cbatTempInfo, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_receive()");
      //exit(1);
    }

    mq_close(mq_batTemp);

    //Temp and Bat info
    memcpy(&cAnimalInfo[4], &cbatTempInfo[0], 1); // TEMPERATURE
    memcpy(&cAnimalInfo[8], &cbatTempInfo[4], 4); // BATTERY
    //memcpy(&cAnimalInfo[13], &cbatTempInfo[8], 1); // ALERTS

    //Open mq_rfSender
    //mq_close(mq_rfSender);
    // pthread_mutex_lock(mutex_queue_rf);
    mq_rfSender = mq_open(MQRFSENDER, O_RDWR); //Read coordinates
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_open()");
      //exit(1);
    }
    mq_getattr(mq_rfSender, &attr);
    cout << "Before:" << endl;
    printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    printf("Maximum message size: %ld\n", attr.mq_msgsize);
    printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);

    while(attr.mq_curmsgs > 0) {
      cout << "maior q um"  << endl;
      msgsz = mq_receive(mq_rfSender, buffer, MAX_MSG_LEN, &sender);
      if (msgsz == -1) {
        perror("CThreadsAnimal::pv_processinInfoHandler In mq_receive()");
        //exit(1);
      }
      mq_getattr(mq_rfSender, &attr);
      // mq_close(mq_rfSender);
    }

    mq_rfSender = mq_open(MQRFSENDER, O_WRONLY); //Read coordinates
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_open()");
      //exit(1);
    }

    mq_rfSender = mq_send(mq_rfSender, cAnimalInfo, 33, msgqGPS_prio);
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_send()");
      //exit(1);
    }
    // pthread_mutex_unlock(mutex_queue_rf);
    mq_close(mq_rfSender);

    cout << "mq_send(mq_rfSender): ";
    for(int i=0; i < 33; i++) {
      printf("%X", cAnimalInfo[i]);
    }
    cout << endl;

    mq_getattr(mq_rfSender, &attr);

    cout << "After:" << endl;
    printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    printf("Maximum message size: %ld\n", attr.mq_msgsize);
    printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);


    pthread_mutex_lock(mutex_endProcessing);
    //Trigger ts_endProcessing
    pthread_cond_signal(ts_endProcessing);
    pthread_mutex_unlock(mutex_endProcessing);
  }
}

void * CThreadsAnimal :: pv_gpsHandler(void *threadid)
{
  char gpsCoordinates[MQGPSLEN];
  unsigned int msgqGPS_prio = 1;
  struct mq_attr attr;

  cout << "thread pv_gpsHandler" << endl;

  while(1)
  {
    //Wait for animal.timeout
    pthread_mutex_lock(mutex_readGPS);
    pthread_cond_wait(ts_readGPS, mutex_readGPS);
    pthread_mutex_unlock(mutex_readGPS);

    cout << "GPS START READING" << endl;
    //Read animal's coordinates
    // if(p_gps->mb_gpsReady())
    // {
    //   cout << "GPS ON AND READY!" << endl;
    //   p_gps->readGps();
    //
    //   if(p_gps->gpsDataStatus()) // Send it to mq_GPS
    //   {
    //     cout << "GPS: Data Valid!" << endl;
         memset(gpsCoordinates, '\0', MQGPSLEN);    // Initialize the string
    //     sprintf(gpsCoordinates, "%f;%f", p_gps->getLatitude(), p_gps->getLongitude());
    //
    //     for(int i=0; i<strlen(gpsCoordinates)+1; i++)
    //     {
    //       printf("%c", gpsCoordinates[i]);
    //     }
    //     cout << endl;

        strcpy(gpsCoordinates, "41.501823;-8.348397");
        mq_close(mq_GPS);
        mq_GPS = mq_open(MQGPS, O_RDWR); //Send coordinates
        if (mq_GPS == (mqd_t)-1) {
          perror("CThreadsAnimal::pv_gpsHandler In mq_open()");
          //exit(1);
        }
        mq_GPS = mq_send(mq_GPS, gpsCoordinates, MQGPSLEN , msgqGPS_prio);
        if (mq_GPS == (mqd_t)-1) {
          perror("CThreadsAnimal::pv_gpsHandler In mq_send()");
          //exit(1);
        }

        // mq_getattr(mq_GPS, &attr);
        // cout << "thread pv_processinInfoHandler" << endl;
        // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
        // printf("Maximum message size: %ld\n", attr.mq_msgsize);
        // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);

        mq_close(mq_GPS);

        pthread_mutex_lock(mutex_GPSReady);
        //Trigger ts_GPSReady
        cout << "Trigger ts_GPSReady"<< endl;
        pthread_cond_signal(ts_GPSReady);
        pthread_mutex_unlock(mutex_GPSReady);
      //
      // }
      // else
      // {
      //   cout << "GPS: Data not Valid! Read again..." << endl;
      // }

    //}

  }

}
