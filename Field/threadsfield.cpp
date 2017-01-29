#include "threadsfield.h"

/***** MUTEX *****/
pthread_mutex_t * mutex_readInfoTcp;
pthread_mutex_t * mutex_sendInfoRF;
pthread_mutex_t * mutex_readInfoRF;
pthread_mutex_t * mutex_endProcess;
pthread_mutex_t * mutex_process;
pthread_mutex_t * mutex_requested;
pthread_mutex_t * mutex_wifi_list;
pthread_mutex_t * mutex_RF;
/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;
pthread_cond_t *ts_readInfoTcp;
pthread_cond_t *ts_endProcess;
pthread_cond_t *ts_process;

/***** SEMAPHORES *****/
sem_t *tsemaphore_wifisend;

/***** QUEUES *****/
mqd_t mq_rf;

/***** Objects *****/
CRFCom *p_rf;
CField *p_field;
CTcpCom *p_tcpcom;
SanimalRegist *animalRegists;
int zone_request=0;
unsigned char type_request=0;
bool manual_id_request=0;
uint16_t manual_id=0;
unsigned char manual_msg[5];


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
  mutex_requested = new pthread_mutex_t();
  int mutex_zone_status=pthread_mutex_init(mutex_requested,NULL);
  mutex_wifi_list = new pthread_mutex_t();
  int mutex_wifi_list_status=pthread_mutex_init(mutex_wifi_list,NULL);
  mutex_RF = new pthread_mutex_t();
  int mutex_RF_status=pthread_mutex_init(mutex_RF,NULL);

    /***** SIGNALS *****/
  ts_readInfoTcp = new pthread_cond_t();
  int ts_readInfoTcp_status=pthread_cond_init(ts_readInfoTcp, NULL);
  ts_sendInfoRF = new pthread_cond_t();
  int ts_sendInfoRF_status=pthread_cond_init(ts_sendInfoRF, NULL);
  ts_readInfoRF = new pthread_cond_t();
  int ts_readInfoRF_status=pthread_cond_init(ts_readInfoRF, NULL);
  ts_process= new pthread_cond_t();
  int ts_process_status=pthread_cond_init(ts_process, NULL);
  ts_endProcess= new pthread_cond_t();
  int ts_endProcess_status=pthread_cond_init(ts_endProcess, NULL);

    /***** QUEUES *****/
    /***** SEMAPHORES *****/
  tsemaphore_wifisend = new sem_t();
  sem_init(tsemaphore_wifisend,0,0);

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
  if(rfS_status || rfR_status || tcpS_status || tcpR_status)
  {
    cout << "ERROR CREATING THREAD" << endl;
    exit(EXIT_FAILURE);
  }

  /***** OBJECTS CONF *****/
  uint8_t addrf[4];
  addrf[0]= 0xFF;
  addrf[1]= 0xFF;
  addrf[2]= 0x00;
  addrf[3]= 0x00;

  p_rf->RFComSetAddR(addrf);
  run();
  pthread_mutex_lock(mutex_process);
  pthread_cond_signal(ts_process);
  pthread_mutex_unlock(mutex_process);

}
CThreadsField::~CThreadsField()
{
  /***** Delete QUEUES *****/
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
  uint16_t id=0;
  uint16_t id_field=1;
  uint16_t aux_manual_id=0;
  bool aux_manual_id_request=false;
  unsigned char aux_type_request='I';
  unsigned char aux_msg[5];
  unsigned char sendMsg[32];
  unsigned char animalAddr[4];

  while (1)
  {
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfoRF);
    pthread_mutex_lock(mutex_requested);
    id=p_field->getAnimal(zone_request);
    aux_manual_id=manual_id;
    aux_manual_id_request=manual_id_request;
    aux_type_request=type_request;
    memcpy(manual_msg,aux_msg,5);
    pthread_mutex_unlock(mutex_requested);
    if(aux_manual_id_request)
    {
      //send info for aux_manual_id with type aux_type_request
      memcpy(&id_field, &sendMsg[0], 2);
      memcpy(&aux_manual_id, &sendMsg[2], 2);
      sendMsg[4]=aux_type_request;
      memcpy(aux_msg,&sendMsg[5],5);
      animalAddr[0]=0xAA;
      animalAddr[1]=0xAA;
      memcpy(&aux_manual_id,&animalAddr[2],2);
    }
    else if(id!=-1)
    {
      // send info for id with type Information
      memcpy(&id_field, &sendMsg[0], 2);
      memcpy(&id, &sendMsg[2], 2);
      sendMsg[4]='I';
      memcpy(aux_msg,&sendMsg[5],5);
      animalAddr[0]=0xAA;
      animalAddr[1]=0xAA;
      memcpy(&id,&animalAddr[2],2);
    }

    pthread_mutex_lock(mutex_RF);
    p_rf->RFComSender(animalAddr,sendMsg);
    pthread_mutex_unlock(mutex_RF);

    pthread_mutex_lock(mutex_readInfoRF);
    pthread_cond_signal(ts_readInfoRF);
    pthread_mutex_unlock(mutex_readInfoRF);
    }
}


void  * CThreadsField::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char aux[32];
  while (1)
  {
    pthread_cond_wait(ts_readInfoRF, mutex_readInfoRF);
    pthread_mutex_lock(mutex_RF);
    p_rf->RFComReceiver(aux);
    pthread_mutex_unlock(mutex_RF);
    delay(500);
    pthread_mutex_lock(mutex_process);
    pthread_cond_signal(ts_process);
    pthread_mutex_unlock(mutex_process);
    cout << aux<< endl;
  }
}

void  * CThreadsField::pv_WIFIComReceiverHandler(void *threadid)
{
  while (1)
  {
    pthread_cond_wait(ts_readInfoTcp, mutex_readInfoTcp);

  }
}

void *CThreadsField::pv_WIFIComSenderHandler(void *threadid)
{
  char buffer[32];
  while (1)
  {
    sem_wait (tsemaphore_wifisend);
    pthread_mutex_lock (mutex_wifi_list);
    p_field->getAnimalInfo(buffer);
    pthread_mutex_unlock (mutex_wifi_list);
    cout << buffer << endl;
  }
}

void *CThreadsField::pv_processAnimalInfoHandler(void *threadid)
{
  int id=0;
  while (1)
  {
    pthread_cond_wait(ts_process, mutex_process);
    //process next animal resquest
    //get id and zone
    p_field->setAnimal(1,GREENZONE);
    pthread_mutex_lock(mutex_requested);
    zone_request=GREENZONE;
    type_request='I';
    manual_id_request=false;
    pthread_mutex_unlock(mutex_requested);

    pthread_mutex_lock(mutex_wifi_list);
    //p_field->setAnimalInfo(id,GREENZONE);
    pthread_mutex_unlock(mutex_wifi_list);

    pthread_mutex_lock(mutex_sendInfoRF);
    pthread_cond_signal(ts_sendInfoRF);
    pthread_mutex_unlock(mutex_sendInfoRF);

    sem_post(tsemaphore_wifisend);

  }
}


void  CThreadsField::run()
{
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);
  //pthread_detach(t_WIFIComReceiver);
  //pthread_detach(t_WIFIComSender);
}
