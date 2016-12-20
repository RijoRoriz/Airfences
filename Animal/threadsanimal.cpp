#include "threadsanimal.h"

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
  int GPS_status = pthread_create(&t_GPS,&thread_attr,pv_GPSHandler,NULL);
  int batT_status = pthread_create(&t_batTemp, &thread_attr,pv_batTempHandler, NULL);

  thread_attr=setAttr(70);
  int pIn_status = pthread_create(&t_processinInfo, &thread_attr, pv_processinInfoHandler, NULL);

  if(rfS_status || rfR_status || shock_status || batT_status || pIn_status || GPS_status)
  {
    //cout << "ERROR CREATING THREAD" << endl;
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
}


void * CThreadsAnimal::pv_RFComSenderHandler(void *threadid)
{
	//while(1)
	//{
		pthread_cond_wait(ts_sendInfo, mutex_sendInfo);
		((CRFCom*)threadid)->RFComSender(NULL, NULL);
	//}
}

void * CThreadsAnimal::pv_RFComReceiverHandler(void *threadid)
{
	//while(1)
	//{
		((CRFCom*)threadid)->RFComReceiver();
		((CRFCom*)threadid)->RFComPrintTAddr();
	//}
}

void * CThreadsAnimal :: pv_shockHandler(void *threadid)
{

}

void * CThreadsAnimal :: pv_batTempHandler(void *threadid)
{
  

}

void * CThreadsAnimal :: pv_processinInfoHandler(void *threadid)
{

}

void * CThreadsAnimal :: pv_gpsHandler(void *threadid)
{


}

int main()
{
	//CThreadsAnimal animal;
	//animal.run();
	mqd_t mq_GPS;
	mqd_t mq_rf;
	mq_rf = mq_open(MQRFCOM, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
	mq_GPS = mq_open(MQGPS, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
	CRFCom teste;
	teste.RFComPrintConf();
	printf("loool\n");
	unsigned char array[32]={0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	teste.RFComSender(NULL,array);
	teste.RFComPrintRPaylo();
	teste.RFComReceiver();
	teste.RFComPrintRPaylo();
	mq_close(mq_GPS);
	mq_close(mq_rf);
	mq_unlink(MQGPS);
	mq_unlink(MQRFCOM);

}
