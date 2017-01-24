#include "threadsfield.h"

/***** MUTEX *****/
pthread_mutex_t * mutex_sendInfoTcp;
pthread_mutex_t * mutex_readInfoTcp;
pthread_mutex_t * mutex_sendInfoRF;
pthread_mutex_t * mutex_readInfoRF;
pthread_mutex_t * mutex_endProcessing;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;
pthread_cond_t *ts_sendInfoTcp;
pthread_cond_t *ts_readInfoTcp;

pthread_cond_t *ts_endProcessing;


/***** QUEUES *****/
mqd_t mq_rf;

/***** Objects *****/
CRFCom *p_rf;
CField *p_field;
CTcpCom *p_tcpcom;
SanimalRegist *animalRegists;

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

    /***** SIGNALS *****/
  ts_sendInfoTcp = new pthread_cond_t();
  int ts_sendInfoTcp_status=pthread_cond_init(ts_sendInfoTcp, NULL);
  ts_readInfoTcp = new pthread_cond_t();
  int ts_readInfoTcp_status=pthread_cond_init(ts_readInfoTcp, NULL);
  ts_sendInfoRF = new pthread_cond_t();
  int ts_sendInfoRF_status=pthread_cond_init(ts_sendInfoRF, NULL);
  ts_readInfoRF = new pthread_cond_t();
  int ts_readInfoRF_status=pthread_cond_init(ts_readInfoRF, NULL);

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
  int tcpS_status = pthread_create(&t_WIFIComSender,&thread_attr,pv_WIFIComSenderHandler,(void*)p_rf);
  int tcpR_status = pthread_create(&t_WIFIComReceiver, &thread_attr,pv_WIFIComReceiverHandler, (void*)p_rf);

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
  while (1)
  {
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfoRF);
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
    cout << "2"<< endl;
    pthread_mutex_lock(mutex_sendInfoRF);
    pthread_cond_signal(ts_sendInfoRF);
    pthread_mutex_unlock(mutex_sendInfoRF);
  }
}

void  * CThreadsField::pv_WIFIComReceiverHandler(void *threadid)
{
  while (1)
  {
    pthread_cond_wait(ts_readInfoTcp, mutex_readInfoTcp);
    cout << "3"<< endl;
  }
}

void *CThreadsField::pv_WIFIComSenderHandler(void *threadid)
{
  while (1)
  {
    pthread_cond_wait(ts_sendInfoTcp, mutex_sendInfoTcp);
    cout << "4"<< endl;
  }
}

void  CThreadsField::run()
{
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);
  //pthread_detach(t_WIFIComReceiver);
  //pthread_detach(t_WIFIComSender);
}
