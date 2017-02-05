#include "threadsanimal.h"

/***** MUTEX *****/
pthread_mutex_t *mutex_sendInfo;
pthread_mutex_t *mutex_readInfo;
pthread_mutex_t *mutex_GPSReady;
pthread_mutex_t *mutex_endProcessing;
pthread_mutex_t *mutex_readGPS;
pthread_mutex_t *mutex_readBatTemp;

pthread_mutex_t *mutex_animalZone;
pthread_mutex_t *mutex_RF;
pthread_mutex_t *mutex_ADC;
pthread_mutex_t *mutex_GPS;


/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;

pthread_cond_t *ts_endProcessing;
pthread_cond_t *ts_GPSReady;
pthread_cond_t *ts_readGPS;
pthread_cond_t *ts_readBatTemp;

/***** Objects *****/
CRFCom *p_rf;
CGps *p_gps;
CAdc *p_adc;
CAnimal *p_animal;
CFieldMap *p_fieldMap;
CLeds *p_leds;

unsigned char msgRfReceiver[MQRFCOMLEN];
unsigned char msgRfSender[MQRFCOMLEN];

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


  mutex_RF = new pthread_mutex_t();
  int mutex_RF_status=pthread_mutex_init(mutex_RF, NULL);
  mutex_ADC = new pthread_mutex_t();
  int mutex_ADC_status=pthread_mutex_init(mutex_ADC, NULL);
  mutex_GPS = new pthread_mutex_t();
  int mutex_GPS_status=pthread_mutex_init(mutex_GPS, NULL);


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


  /***** OBJECT *****/
  p_rf = new CRFCom();
  p_animal = new CAnimal();
  p_fieldMap = new CFieldMap();
  p_adc = new CAdc();
  p_leds = new CLeds();

  #if GPS
  p_gps = new CGps();
  p_gps->initGps();
  #endif

  p_animal->m_setAnimalTimeout(REDZONE);
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
  unsigned char addrT[4] = {0xFF, 0xFF, 0x00, 0x01};

  #if DEBUG
  cout << "thread pv_RFComSenderHandler" << endl;
  #endif

  while(1)
  {
    //Wait for ts_sendInfoRF
    pthread_mutex_lock(mutex_sendInfo);
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfo);
    pthread_mutex_unlock(mutex_sendInfo);

    cout << "Ready to send message" << endl;

    switch (msgRfReceiver[4]) { //Command Type
      case 'R':
      case 'N': //Send message received to field to know that the message was delivered
      case 'C':
      pthread_mutex_lock(mutex_RF);
      for(int i=0; i < MQRFCOMLEN; i++) {
        msgRfSender[i] = static_cast<unsigned char>(msgRfReceiver[i]);
      }
      pthread_mutex_unlock(mutex_RF);
      break;

      case 'I': // Request Info
      uint16_t idAnimal, idField;
      int zone;
      char type;
      float bat,temp, lat1, long1;
      pthread_mutex_lock(mutex_RF);
      cout << "REQUEST INFO " << endl;
      memcpy(&msgRfSender[0], &msgRfReceiver[0], 2); //ID_Field
      memcpy(&idField, &msgRfReceiver[0], 2); //ID_Field
      memcpy(&msgRfSender[2], &msgRfReceiver[2], 2); //ID_Animal
      memcpy(&idAnimal, &msgRfReceiver[2], 2); //ID_Animal
      memcpy(&msgRfSender[4], &msgRfReceiver[4], 1); //Command 'I'
      memcpy(&type, &msgRfReceiver[4], 1); //Command 'I'


      // if(msgRfReceiver[5] == 1) { //Request Temperature
      //   memcpy(&msgRfSender[5], &msgRfReceiver[5],4);
        memcpy(&temp, &msgRfSender[5],4);
      // }
      // if(msgRfReceiver[6] == 1) { //Request Battery
      //   memcpy(&msgRfSender[9], &msgRfReceiver[9],4);
        memcpy(&bat, &msgRfSender[9],4);
      // }
      // if(msgRfReceiver[7] == 1) { //Request GPS
      //   memcpy(&msgRfSender[13], &msgRfReceiver[13], 1);
         memcpy(&zone, &msgRfSender[13], 1);
      //   memcpy(&msgRfSender[14], &msgRfReceiver[14], 4);
        memcpy(&lat1, &msgRfSender[14], 4);
      //   memcpy(&msgRfSender[18], &msgRfReceiver[18], 4);
        memcpy(&long1, &msgRfSender[18], 4);
      // }
      pthread_mutex_unlock(mutex_RF);
      #if INFO
      cout << fixed << setprecision(7)
      << "ID ANIMAL: " << idAnimal << endl
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

    pthread_mutex_lock(mutex_RF);
    #if INFO
    cout << "msgRfSender: ";
    for(int i=0; i < MQRFCOMLEN; i++) {
      printf("%X ", msgRfSender[i]);
    }
    cout << endl;
    #endif
    //Send requested information
    p_rf->RFComSender(addrT, msgRfSender);
    // p_rf->RFComPrintTPaylo(); //Print message transmited
    pthread_mutex_unlock(mutex_RF);

    pthread_mutex_lock(mutex_readInfo);
    pthread_cond_signal(ts_readInfoRF);
    pthread_mutex_unlock(mutex_readInfo);
  }
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
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

    //Configure Receiving ADDR
    uint16_t id=p_animal->mui_getAnimalID();
    addrR[2] = (id & 0xFF00) >> 8;
    addrR[3] = id & 0x00FF;
    p_rf->RFComSetAddR(addrR);

    cout << "Waiting message" << endl;
    // pthread_mutex_lock(mutex_RF);
    p_rf->RFComReceiver(msgRfReceiver);  //Wait for a message
    // p_rf->RFComPrintRPaylo(); //Print message received
    // pthread_mutex_unlock(mutex_RF);

    #if INFO
    cout << "MESSAGE RECEIVEID: ";
    for(int i=0; i < MQRFCOMLEN; i++)
    {
      printf("%X ", msgRfReceiver[i] );
    }
    cout << endl;
    #endif

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
        // p_gps->readGps();
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
    cout << "Animal Zone: " << iAnimalZone << endl;
    //Turn on the LED zone
    p_leds->m_turnON_LedZone(iAnimalZone);
    pthread_mutex_unlock(mutex_animalZone);

  }
}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  float ftemperature, fbatteryLevel;

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

pthread_mutex_lock(mutex_ADC);
    //Read animal's temperature
    p_adc->readTemperature();
    ftemperature = p_adc->getTemperature();
    // cout << "TEMP: " << ftemperature << endl;

    //Read animal's battery level
    p_adc->readBatteryLevel();
    fbatteryLevel = p_adc->getBatteryLevel();
    // cout << "BAT: " << fbatteryLevel << endl;
pthread_mutex_unlock(mutex_ADC);

    if(fbatteryLevel > 50.0) {
      //Turn on Power LED
      p_leds->m_BatteryLedStatus(true);
    }
    else if(fbatteryLevel < 25.0) { //Low battery level
      //Blink Power LED
      cout << "Low Battery Level" << endl;
      p_leds->m_batteryWarning();
    }
    else if(fbatteryLevel < 10.0) {
      // system("poweroff");
    }
    else { //Full battery
      //Power LED off
      p_leds->m_BatteryLedStatus(false);
    }
  }
}

void * CThreadsAnimal :: pv_processinInfoHandler(void *threadid)
{
  int iAnimalZone;
  float latitude, longitude;
  float temperature, batteryLevel;

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

    latitude = 41.501823;
    longitude = -8.348397;

#if GPS
    pthread_mutex_lock(mutex_GPS);
    latitude = p_gps->getLatitude();
    longitude = p_gps->getLongitude();
    // cout << setprecision(6) << fixed
    // << "Latitude: " << latitude << endl
    // << "Longitude: " << longitude << endl;
    pthread_mutex_unlock(mutex_GPS);
#endif

pthread_mutex_lock(mutex_animalZone);
    //Check and Return AnimalZone
    //GPS info 41.501823;-8.348397
    iAnimalZone = p_fieldMap->mi_checkAnimalZone(latitude, longitude);
    cout << "PI_AnimalZone: " << iAnimalZone << endl;
    //Set the animal zone and the timer of the zone
    p_animal->m_setAnimalZone(iAnimalZone);
    p_animal->m_setAnimalTimeout(iAnimalZone);
pthread_mutex_unlock(mutex_animalZone);

pthread_mutex_lock(mutex_ADC);
  temperature = p_adc->getTemperature();
  batteryLevel = p_adc->getBatteryLevel();
pthread_mutex_unlock(mutex_ADC);

pthread_mutex_lock(mutex_RF);
    memcpy(&msgRfSender[13], &iAnimalZone, 1);
    memcpy(&msgRfSender[14], &latitude, 4);
    memcpy(&msgRfSender[18], &longitude, 4);
    //Temp and Bat info
    memcpy(&msgRfSender[5], &temperature, 4); // TEMPERATURE
    memcpy(&msgRfSender[9], &batteryLevel, 4); // BATTERY

    cout << "PROCESSING: ";
    for(int i=0; i< 32; i++) {
      printf("%X ", msgRfSender[i]);
    }
    cout << endl;
pthread_mutex_unlock(mutex_RF);
// delay(2);

    pthread_mutex_lock(mutex_endProcessing);
    //Trigger ts_endProcessing
    pthread_cond_signal(ts_endProcessing);
    pthread_mutex_unlock(mutex_endProcessing);
  }
}

void * CThreadsAnimal :: pv_gpsHandler(void *threadid)
{
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
#if GPS
    // Read animal's coordinates
    if(p_gps->mb_gpsReady())
    {
      cout << "GPS ON AND READY!" << endl;
      p_gps->readGps();

      if(p_gps->gpsDataStatus()) // Send it to mq_GPS
      {
        cout << "GPS: Data Valid!" << endl;
#endif
        // delay(10);
        pthread_mutex_lock(mutex_GPSReady);
        //Trigger ts_GPSReady
        cout << "Trigger ts_GPSReady"<< endl;
        pthread_cond_signal(ts_GPSReady);
        pthread_mutex_unlock(mutex_GPSReady);
#if GPS
      }
      else
      {
        cout << "GPS: Data not Valid! Read again..." << endl;
      }

    }
#endif

  }

}
