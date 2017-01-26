#include "threadsfield.h"

/***** MUTEX *****/
pthread_mutex_t * mutex_sendInfoTcp;
pthread_mutex_t * mutex_readInfoTcp;
pthread_mutex_t * mutex_sendInfoRF;
pthread_mutex_t * mutex_readInfoRF;
pthread_mutex_t * mutex_endProcess;
pthread_mutex_t * mutex_process;
pthread_mutex_t * mutex_zone_requested;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;
pthread_cond_t *ts_sendInfoTcp;
pthread_cond_t *ts_readInfoTcp;
pthread_cond_t *ts_endProcess;
pthread_cond_t *ts_process;


/***** QUEUES *****/
mqd_t mq_rf;

/***** Objects *****/
CRFCom *p_rf;
CField *p_field;
CTcpCom *p_tcpcom;
SanimalRegist *animalRegists;
int zone_requested=0;

CThreadsField::CThreadsField()
{
    /***** MUTEX *****/
  mutex_sendInfoTcp = new pthread_mutex_t();
  int mutex_sendInfoTcp_status=pthread_mutex_init(mutex_sendInfoTcp,NULL);
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
  mutex_zone_requested = new pthread_mutex_t();
  int mutex_zone_status=pthread_mutex_init(mutex_zone_requested,NULL);

    /***** SIGNALS *****/
  ts_sendInfoTcp = new pthread_cond_t();
  int ts_sendInfoTcp_status=pthread_cond_init(ts_sendInfoTcp, NULL);
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
  pthread_mutex_lock(mutex_sendInfoRF);
  pthread_cond_signal(ts_sendInfoRF);
  pthread_mutex_unlock(mutex_sendInfoRF);

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
  int list=0;
  while (1)
  {
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfoRF);
    pthread_mutex_lock(mutex_zone_requested);
    list=p_field->getAnimal(zone_requested);
    pthread_mutex_unlock(mutex_zone_requested);
    memcpy(&mui_idField, &message[0], 2); //Set FieldID
    //p_rf->RFComSender();
    pthread_mutex_lock(mutex_readInfoRF);
    pthread_cond_signal(ts_readInfoRF);
    pthread_mutex_unlock(mutex_readInfoRF);
  }
}

void  * CThreadsField::pv_RFComReceiverHandler(void *threadid)
{
  while (1)
  {
    pthread_cond_wait(ts_readInfoRF, mutex_readInfoRF);
    pthread_mutex_lock(mutex_process);
    pthread_cond_signal(ts_process);
    pthread_mutex_unlock(mutex_process);
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
  while (1)
  {
    pthread_cond_wait(ts_sendInfoTcp, mutex_sendInfoTcp);
  }
}

void *CThreadsField::pv_processAnimalInfoHandler(void *threadid)
{
  while (1)
  {
    pthread_cond_wait(ts_process, mutex_process);
    p_field->setAnimal(1, GREENZONE);
    p_field->setAnimal(2, GREENZONE);
    p_field->setAnimal(3, GREENZONE);
    p_field->setAnimal(4, GREENZONE);
    p_field->getAnimal(YELLOWZONE);
    cout << p_field->getAnimal(GREENZONE) << endl;
    cout << p_field->getAnimal(GREENZONE) << endl;
    cout << p_field->getAnimal(GREENZONE) << endl;
    cout << p_field->getAnimal(GREENZONE) << endl;
    cout << p_field->getAnimal(GREENZONE) << endl;
    //pthread_mutex_lock(mutex_sendInfoRF);
    //pthread_cond_signal(ts_sendInfoRF);
    //pthread_mutex_unlock(mutex_sendInfoRF);
  }
}


void  CThreadsField::run()
{
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);
  //pthread_detach(t_WIFIComReceiver);
  //pthread_detach(t_WIFIComSender);
}
