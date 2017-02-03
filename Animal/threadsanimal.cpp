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


/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;

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
CLeds *p_leds;

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

  /***********************************************************************/

  /***** SIGNALS *****/
  ts_sendInfoRF = new pthread_cond_t();
  int ts_sendInfoRF_status=pthread_cond_init(ts_sendInfoRF, NULL);
  ts_readInfoRF = new pthread_cond_t();
  int ts_readInfoRF_status=pthread_cond_init(ts_readInfoRF, NULL);
  ts_endProcessing = new pthread_cond_t();
  int ts_endProcessing_status=pthread_cond_init(ts_endProcessing, NULL);
  ts_GPSReady = new pthread_cond_t();
  int ts_GPSReady_status=pthread_cond_init(ts_GPSReady, NULL);
  ts_readGPS = new pthread_cond_t();
  int ts_readGPS_status=pthread_cond_init(ts_readGPS, NULL);
  ts_readBatTemp = new pthread_cond_t();
  int ts_readBatTemp_status=pthread_cond_init(ts_readBatTemp, NULL);
  /***********************************************************************/

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
  p_leds = new CLeds();

  p_animal->m_setAnimalTimeout(REDZONE);
  // p_gps->initGps();
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
  pthread_cond_signal(ts_readInfoRF);
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
  its.it_value.tv_sec = 3; //10 seconds
  its.it_interval.tv_sec = 3; //10 seconds

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
    //Wait for ts_sendInfoRF
    pthread_mutex_lock(mutex_sendInfo);
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfo);
    pthread_mutex_unlock(mutex_sendInfo);

    cout << "Ready to send message" << endl;

    //open mq_rfReceiver - command/request received
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
      printf("%X ", cCommand[i]);
    }
    cout << endl;
    #endif

    //Open mq_rfSender - message to send (request)
    mq_close(mq_rfSender);
    mq_rfSender = mq_open(MQRFSENDER, O_RDONLY);
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_RFComSenderHandler In mq_open()");
      //exit(1);
    }
    msgsz = mq_receive(mq_rfSender, cAnimalInfo, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
      perror("CThreadsAnimal::pv_RFComSenderHandler In mq_receive()");
      //exit(1);
    }
    mq_close(mq_rfSender);

    // #if DEBUG
    cout << "mq_receive(mq_rfSender): ";
    for(int i=0; i < 33; i++) {
      printf("%X ", cAnimalInfo[i]);
    }
    cout << endl;
    // #endif

    switch (cCommand[4]) { //Command Type
      case 'R':
      case 'N': //Send message received to field to know that the message was delivered
      case 'C':
      for(int i=0; i < MQRFCOMLEN; i++) {
        requestedInfo[i] = static_cast<unsigned char>(cCommand[i]);
      }
      break;

      case 'I': // Request Info
      unsigned char test[32];
      memset(test, '\0', 32);
      for(int i=0; i < 32; i++) {
        test[i] = static_cast<unsigned char>(cAnimalInfo[i]);
      }
      uint16_t idAnimal, idField;
      int zone;
      char type;
      float bat,temp, lat1, long1;
      memcpy(&requestedInfo[0], &cCommand[0], 2); //ID_Field
      memcpy(&idField, &cCommand[0], 2); //ID_Field
      memcpy(&requestedInfo[2], &cCommand[2], 2); //ID_Animal
      memcpy(&idAnimal, &cCommand[2], 2); //ID_Animal
      memcpy(&requestedInfo[4], &cCommand[4], 1); //Command 'I'
      memcpy(&type, &cCommand[4], 1); //Command 'I'

      if(cCommand[5] == 1) { //Request Temperature
        memcpy(&requestedInfo[5], &cAnimalInfo[5],4);
        memcpy(&temp, &cAnimalInfo[5],4);
      }
      if(cCommand[6] == 1) { //Request Battery
        memcpy(&requestedInfo[9], &cAnimalInfo[9],4);
        memcpy(&bat, &cAnimalInfo[9],4);
      }
      if(cCommand[7] == 1) { //Request GPS
        memcpy(&requestedInfo[13], &cAnimalInfo[13], 1);
        memcpy(&zone, &cAnimalInfo[13], 1);
        memcpy(&requestedInfo[14], &cAnimalInfo[14], 4);
        memcpy(&lat1, &cAnimalInfo[14], 4);
        memcpy(&requestedInfo[18], &cAnimalInfo[18], 4);
        memcpy(&long1, &cAnimalInfo[18], 4);
      }
      #if DEBUG
      cout << "ID ANIMAL: " << idAnimal << endl
      << "ID FIELD: " << idField << endl
      << "COMMAND: " << type << endl
      << "BAT: " << bat << endl
      << "TEMP: " << temp << endl
      << "ZONE: " << zone << endl
      << "LAT: " << lat1 << endl
      << "LONG: " << long1 << endl;
      #endif
      break;

      default:
      cout << "CThreadsAnimal::pv_RFComSenderHandler - No command received!" << endl;
      break;
    }

    #if DEBUG
    cout << "requestedInfo: ";
    for(int i=0; i < 33; i++) {
      printf("%X ", requestedInfo[i]);
    }
    cout << endl;
    #endif

    //Send requested information
    p_rf->RFComSender(addrT, requestedInfo);
    p_rf->RFComPrintTPaylo(); //Print message transmited
    pthread_mutex_lock(mutex_readInfo);
    pthread_cond_signal(ts_readInfoRF);
    pthread_mutex_unlock(mutex_readInfo);
  }
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char msgRfReceiver[MQRFCOMLEN];
  char msg2queue[MQRFCOMLEN];
  unsigned int msgqRF_prio = 1;
  unsigned char addrR[4] = {0xAA, 0xAA};

  #if DEBUG
  cout << "thread pv_RFComReceiverHandler" << endl;
  #endif

  p_rf->RFComSetAddR(addrR);

  while(1)
  {
    pthread_mutex_lock(mutex_readInfo);
    pthread_cond_wait(ts_readInfoRF, mutex_readInfo);
    pthread_mutex_unlock(mutex_readInfo);

    memset(msgRfReceiver, '\0', MQRFCOMLEN);
    memset(msg2queue, '\0', MQRFCOMLEN);

    //Configure Receiving ADDR
    uint16_t id=p_animal->mui_getAnimalID();
    addrR[2] = (id & 0xFF00) >> 8;
    addrR[3] = id & 0x00FF;
    p_rf->RFComSetAddR(addrR);

    cout << "Waiting message" << endl;
    p_rf->RFComReceiver(msgRfReceiver);  //Wait for a message
    // p_rf->RFComPrintRPaylo(); //Print message received

    // #if DEBUG
    cout << "MESSAGE RECEIVEID: ";
    for(int i=0; i < MQRFCOMLEN; i++)
    {
      printf("%X ", msgRfReceiver[i] );
    }
    cout << endl;
    // #endif

    //Check command
    switch (msgRfReceiver[4]) { //Command Type
      //Reset Command "ID_Field,ID_Animal,R,Temperature,Battery,GPS,RF,State"
      case 'R':
      cout << "RESET COMMAND" << endl;
      if(msgRfReceiver[5] == 1) { //Reset Temperature
        p_adc->readTemperature();
      }
      if(msgRfReceiver[6] == 1) { //Reset Battery
        p_adc->readBatteryLevel();
      }
      if(msgRfReceiver[7] == 1) { //Reset GPS
        p_gps->readGps();
      }
      if(msgRfReceiver[8] == 1) { //Reset RF
        //Restart RF
        p_rf->RFComReset();
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
      p_animal->m_setAnimalConf(msgRfReceiver);
      //Config Animal GreenZone
      p_fieldMap->m_configureMap(p_animal->mssq_getAnimalFenceLimits());
      break;

      default:
      break;
    }

    for(int i=0; i < MQRFCOMLEN; i++) {
      msg2queue[i]=static_cast<char>(msgRfReceiver[i]);
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
      printf("%X ", msg2queue[i]);
    }
    cout << endl;
    #endif

    cout << "Signal ts_sendInfoRF" << endl;
    pthread_mutex_lock(mutex_sendInfo);
    //Trigger ts_sendInfoRF
    pthread_cond_signal(ts_sendInfoRF);
    pthread_mutex_unlock(mutex_sendInfo);

  }
}

void * CThreadsAnimal :: pv_shockHandler(void *threadid)
{
  int iAnimalZone;

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
    p_leds->m_turnON_LedZone(iAnimalZone);

  }
}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  float ftemperature, fbatteryLevel;
  char msg_batTemp[MQBATTEMPLEN];
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
    // cout << "TEMP: " << ftemperature << endl;

    memcpy(&msg_batTemp[0], &ftemperature, 4);

    // if(ftemperature < (TEMP_ANIMAL - 0.5)) { //Alert low temperature
    //   cout << "Low temperature" << endl;
    // }
    // else if(ftemperature > (TEMP_ANIMAL + 0.5)) { //Alert high temperature
    //   cout << "High temperature" << endl;
    // }
    // else {
    //   cout << "Normal temperature" << endl;
    // }

    //Read animal's battery level
    p_adc->readBatteryLevel();
    fbatteryLevel = p_adc->getBatteryLevel();
    // cout << "BAT: " << fbatteryLevel << endl;

    memcpy(&msg_batTemp[4], &fbatteryLevel, 4);

    if(fbatteryLevel > 50.0) {
      //Turn on Power LED
      p_leds->m_BatteryLedStatus(true);
      // system("shutdown -H now");
      // execl("/sbin/poweroff", "poweroff", NULL);
    }
    else if(fbatteryLevel < 25.0) { //Low battery level
      //Blink Power LED
      cout << "Low Battery Level" << endl;
      system("poweroff");
      p_leds->m_batteryWarning();
    }
    else if(fbatteryLevel < 10.0) {
      system("shutdown -P now");
    }
    else { //Full battery
      //Power LED off
      p_leds->m_BatteryLedStatus(false);
    }

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

    mq_batTemp = mq_send(mq_batTemp, msg_batTemp, MQBATTEMPLEN, 1);
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
  char cAnimalInfo[MQRFCOMLEN];
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
    memset(cAnimalInfo, '\0', MQRFCOMLEN);

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
    mq_close(mq_GPS);

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
    memcpy(&cAnimalInfo[13], &iAnimalZone, 1);
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
    memcpy(&cAnimalInfo[5], &cbatTempInfo[0], 4); // TEMPERATURE
    memcpy(&cAnimalInfo[9], &cbatTempInfo[4], 4); // BATTERY

    //Open mq_rfSender
    //mq_close(mq_rfSender);
    mq_rfSender = mq_open(MQRFSENDER, O_RDWR); //Read coordinates
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_open()");
      //exit(1);
    }
    mq_getattr(mq_rfSender, &attr);
    // cout << "Before:" << endl;
    // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    // printf("Maximum message size: %ld\n", attr.mq_msgsize);
    // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);

    while(attr.mq_curmsgs > 0) {
      // cout << "maior q um"  << endl;
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
    mq_rfSender = mq_send(mq_rfSender, cAnimalInfo, MQRFCOMLEN, msgqGPS_prio);
    if (mq_rfSender == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_send()");
      //exit(1);
    }
    mq_close(mq_rfSender);

    cout << "mq_send(mq_rfSender): ";
    for(int i=0; i < 33; i++) {
      printf("%X", cAnimalInfo[i]);
    }
    cout << endl;

    // mq_getattr(mq_rfSender, &attr);

    // #if DEBUG
    // cout << "After:" << endl;
    // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    // printf("Maximum message size: %ld\n", attr.mq_msgsize);
    // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);
    // #endif

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

  #if DEBUG
  cout << "thread pv_gpsHandler" << endl;
  #endif

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
        // sprintf(gpsCoordinates, "%f;%f", p_gps->getLatitude(), p_gps->getLongitude());
    //
    //     cout << "gpsCoordinates: " << gpsCoordinates << endl;
    //
    //     // for(int i=0; i<strlen(gpsCoordinates)+1; i++)
    //     // {
    //     //   printf("%c", gpsCoordinates[i]);
    //     // }
    //     // cout << endl;
    //
        strcpy(gpsCoordinates, "41.501823;-8.348397");
    //     // mq_close(mq_GPS);
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

    //     // mq_getattr(mq_GPS, &attr);
    //     // cout << "thread pv_processinInfoHandler" << endl;
    //     // printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    //     // printf("Maximum message size: %ld\n", attr.mq_msgsize);
    //     // printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);
    //
        mq_close(mq_GPS);
    //
        pthread_mutex_lock(mutex_GPSReady);
        //Trigger ts_GPSReady
        cout << "Trigger ts_GPSReady"<< endl;
        pthread_cond_signal(ts_GPSReady);
        pthread_mutex_unlock(mutex_GPSReady);
    //
    //   }
    //   else
    //   {
    //     cout << "GPS: Data not Valid! Read again..." << endl;
    //   }
    //
    // }

  }

}
