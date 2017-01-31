#include "threadsfield.h"

/***** MUTEX *****/
pthread_mutex_t * mutex_readInfoTcp;
pthread_mutex_t * mutex_sendInfoRF;
pthread_mutex_t * mutex_readInfoRF;
pthread_mutex_t * mutex_endProcess;
pthread_mutex_t * mutex_process;
pthread_mutex_t * mutex_wifi_list;
pthread_mutex_t * mutex_RF;
/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;
pthread_cond_t *ts_readInfoWifi;
pthread_cond_t *ts_endProcess;
pthread_cond_t *ts_process;

/***** SEMAPHORES *****/
sem_t *tsem_sendInfoWifi;

/***** QUEUES *****/
mqd_t mq_rf;
mqd_t mq_wifiReceiver;

/***** Objects *****/
CRFCom *p_rf;
CField *p_field;
CTcpCom *p_tcpcom;
SanimalRegist *animalRegists;
unsigned char msgRfSender[32];



CThreadsField::CThreadsField()
{
  /***** MUTEX *****/
  mutex_readInfoTcp = new pthread_mutex_t();
  int mutex_readInfoTcp_status=pthread_mutex_init(mutex_readInfoTcp,NULL);
  mutex_sendInfoRF = new pthread_mutex_t();
  int mutex_sendInfoRF_status=pthread_mutex_init(mutex_sendInfoRF,NULL);
  mutex_readInfoRF = new pthread_mutex_t();
  int mutex_readInfoRF_status=pthread_mutex_init(mutex_sendInfoRF,NULL);
  mutex_process = new pthread_mutex_t();
  int mutex_process_status=pthread_mutex_init(mutex_process,NULL);
  mutex_endProcess = new pthread_mutex_t();
  int mutex_endProcess_status=pthread_mutex_init(mutex_endProcess,NULL);
  mutex_wifi_list = new pthread_mutex_t();
  int mutex_wifi_list_status=pthread_mutex_init(mutex_wifi_list,NULL);
  mutex_RF = new pthread_mutex_t();
  int mutex_RF_status=pthread_mutex_init(mutex_RF,NULL);

  /***** SIGNALS *****/
  ts_readInfoWifi = new pthread_cond_t();
  int ts_readInfoTcp_status=pthread_cond_init(ts_readInfoWifi, NULL);
  ts_sendInfoRF = new pthread_cond_t();
  int ts_sendInfoRF_status=pthread_cond_init(ts_sendInfoRF, NULL);
  ts_readInfoRF = new pthread_cond_t();
  int ts_readInfoRF_status=pthread_cond_init(ts_readInfoRF, NULL);
  ts_process= new pthread_cond_t();
  int ts_process_status=pthread_cond_init(ts_process, NULL);
  ts_endProcess= new pthread_cond_t();
  int ts_endProcess_status=pthread_cond_init(ts_endProcess, NULL);

  /***** QUEUES *****/
  mq_close(mq_wifiReceiver);
  mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR | O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);

  if (mq_wifiReceiver == (mqd_t)-1) {
    perror("CThreadsField::CThreadsField In mq_open()");
    //exit(1);
  }


  /***** SEMAPHORES *****/
  tsem_sendInfoWifi = new sem_t();
  sem_init(tsem_sendInfoWifi,0,0);

  /***** OBJECT *****/
  p_field = new CField();
  p_rf = new CRFCom();
  p_tcpcom = new CTcpCom();
  animalRegists=NULL;

  /***** THREADS *****/
  pthread_attr_t thread_attr=setAttr(60); //
  int rfS_status = pthread_create(&t_RFComSender,&thread_attr,pv_RFComSenderHandler,(void*)p_rf);
  int rfR_status = pthread_create(&t_RFComReceiver, &thread_attr,pv_RFComReceiverHandler, (void*)p_rf);
  int tcpS_status = pthread_create(&t_WIFIComSender,&thread_attr,pv_WIFIComSenderHandler,(void*)p_tcpcom);
  int tcpR_status = pthread_create(&t_WIFIComReceiver, &thread_attr,pv_WIFIComReceiverHandler, (void*)p_tcpcom);
  int process_status = pthread_create(&t_processAnimalInfo, &thread_attr,pv_processAnimalInfoHandler, (void*)p_field);

  if(rfS_status || rfR_status || tcpS_status || tcpR_status) {
    cout << "ERROR CREATING THREAD" << endl;
    exit(EXIT_FAILURE);
  }

  /***** OBJECTS CONF *****/
  unsigned char addrR[4] = {0xFF, 0xFF, 0x00, 0x01};

  p_rf->RFComSetAddR(addrR);
  run();
  pthread_mutex_lock(mutex_process);
  pthread_cond_signal(ts_process);
  pthread_mutex_unlock(mutex_process);

}
CThreadsField::~CThreadsField()
{
  /***** Delete QUEUES *****/
  mq_close(mq_wifiReceiver);
  /***** Delete Threads *****/
  /***** Delete Objects *****/


}
pthread_attr_t CThreadsField::setAttr(int prio)
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

void * CThreadsField::pv_RFComSenderHandler(void *threadid)
{
  unsigned char animalAddr[4];

  while (1)
  {
    pthread_mutex_lock(mutex_sendInfoRF);
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfoRF);
    pthread_mutex_unlock(mutex_sendInfoRF);

    animalAddr[0]=0xAA;
    animalAddr[1]=0xAA;
    animalAddr[2]=0x00;
    animalAddr[3]=0x01;
    pthread_mutex_lock(mutex_RF);
    p_rf->RFComSender(animalAddr,msgRfSender);
    p_rf->RFComPrintTPaylo();
    pthread_mutex_unlock(mutex_RF);

    pthread_mutex_lock(mutex_readInfoRF);
    pthread_cond_signal(ts_readInfoRF);
    pthread_mutex_unlock(mutex_readInfoRF);
    }
}


void  * CThreadsField::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char msgRfReceiver[32];
  char msgWifiSender[32];
  int trys=0;
  bool receved=false;
  while (1)
  {
    pthread_mutex_lock(mutex_readInfoRF);
    pthread_cond_wait(ts_readInfoRF, mutex_readInfoRF);
    pthread_mutex_unlock(mutex_readInfoRF);

    memset(msgRfReceiver, '\0', 32);
    memset(msgWifiSender, '\0', 32);

    pthread_mutex_lock(mutex_RF);
    receved=p_rf->RFComReceiver(msgRfReceiver);
    pthread_mutex_unlock(mutex_RF);
    if(!receved) //Message not received
    {
      if(trys<3)
      {
        trys++;
        cout << "timeout"<< endl;
        pthread_mutex_lock(mutex_sendInfoRF);
        pthread_cond_signal(ts_sendInfoRF);
        pthread_mutex_unlock(mutex_sendInfoRF);
      }
      else
      {
        cout << "not found"<< endl;
        trys=0;

      }

    }
    else
    { //Message Received
      trys=0;
      receved=false;
      pthread_mutex_lock(mutex_RF);
      //p_rf->RFComPrintRPaylo();
      for(int i=0;i<23;i++)
      {
        printf("%x ", msgRfReceiver[i] );
      }
      //Falta comparar strings

      //Information requested (Command 'I')
      for(int i=0; i < 32; i++) {
        msgWifiSender[i] = static_cast<char>(msgRfReceiver[i]);
      }
      pthread_mutex_unlock(mutex_RF);

      pthread_mutex_lock(mutex_wifi_list);
      p_field->setAnimalInfo(msgWifiSender);
      pthread_mutex_unlock(mutex_wifi_list);

      sem_post(tsem_sendInfoWifi);

    }
    pthread_mutex_lock(mutex_process);
    pthread_cond_signal(ts_process);
    pthread_mutex_unlock(mutex_process);
  }
}

void  * CThreadsField::pv_WIFIComReceiverHandler(void *threadid)
{
  char cReadWifi[32];
  struct mq_attr attr;
  unsigned int msgqWifi_prio = 1;

  while (1)
  {
    pthread_mutex_lock(mutex_readInfoTcp);
    pthread_cond_wait(ts_readInfoWifi, mutex_readInfoTcp);
    pthread_mutex_unlock(mutex_readInfoTcp);

    memset(cReadWifi, '\0', 32);
    //Get message from wifi
    //Copy to cReadWifi

    mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR);
    if (mq_wifiReceiver == (mqd_t)-1) {
      perror("CThreadsField::pv_WIFIComReceiverHandler In mq_open()");
      //exit(1);
    }

    mq_wifiReceiver = mq_send(mq_wifiReceiver, cReadWifi, 33, msgqWifi_prio);
    if (mq_wifiReceiver == (mqd_t)-1) {
      perror("CThreadsAnimal::pv_processinInfoHandler In mq_send()");
      //exit(1);
    }
    mq_close(mq_wifiReceiver);



  }
}

void *CThreadsField::pv_WIFIComSenderHandler(void *threadid)
{
  char buffer[32];
  uint16_t idAnimal, idField;
  char command;
  float temperature, baterry;
  int zone;
  float latitude, longitude;

  while (1)
  {
    sem_wait (tsem_sendInfoWifi);
    pthread_mutex_lock (mutex_wifi_list);
    p_field->getAnimalInfo(buffer);
    pthread_mutex_unlock (mutex_wifi_list);

    cout << buffer << endl;
  }
}

void *CThreadsField::pv_processAnimalInfoHandler(void *threadid)
{
  uint16_t id=0;
  unsigned char type='I';
  uint16_t id_field = FIELDADDRT;

  char buffer[MAX_MSG_LEN];
  unsigned int sender;
  struct mq_attr attr;


  while (1)
  {
    pthread_mutex_lock(mutex_process);
    pthread_cond_wait(ts_process, mutex_process);
    pthread_mutex_unlock(mutex_process);

    memset(buffer, '\0', MAX_MSG_LEN);

    mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR);
    if (mq_wifiReceiver == (mqd_t)-1) {
      perror("CThreadsField::pv_processAnimalInfoHandler In mq_open()");
      //exit(1);
    }

    mq_getattr(mq_wifiReceiver, &attr);

    //Have a commando to send by wifi ('N' or 'C')
    if(attr.mq_curmsgs > 0) {
      int msgsz = mq_receive(mq_wifiReceiver, buffer, MAX_MSG_LEN, &sender);
      if (msgsz == -1) {
        perror("CThreadsField::pv_processAnimalInfoHandler In mq_receive()");
        //exit(1);
      }
      mq_close(mq_wifiReceiver);
    }
    else {
      mq_close(mq_wifiReceiver);

      //process next animal resquest
      //get id and zone
      p_field->setAnimal(1,GREENZONE);

      //pthread_mutex_lock(mutex_wifi_list);
      //p_field->setAnimalInfo(id,GREENZONE);
      //pthread_mutex_unlock(mutex_wifi_list);

      id=p_field->getAnimal(GREENZONE);
      pthread_mutex_lock(mutex_RF);
      memcpy(&msgRfSender[0],&id_field, 2);
      memcpy(&msgRfSender[2],&id, 2);
      msgRfSender[4]=type;
      // memcpy(&msg[5],aux_msg,5);
      // memcpy(&msg[],&animalAddr[2],2);
      msgRfSender[5]=1;
      msgRfSender[6]=1;
      msgRfSender[7]=1;
      pthread_mutex_unlock(mutex_RF);
    }


    pthread_mutex_lock(mutex_sendInfoRF);
    pthread_cond_signal(ts_sendInfoRF);
    pthread_mutex_unlock(mutex_sendInfoRF);


    //sem_post(tsem_sendInfoWifi);
  }
}


void  CThreadsField::run()
{
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);
  //pthread_detach(t_WIFIComReceiver);
  //pthread_detach(t_WIFIComSender);
}
