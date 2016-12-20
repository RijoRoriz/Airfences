#ifndef THREADSANIMAL_H_
#define THREADSANIMAL_H_
#include "defines.h"
#include "NRF905.h"

/***** MUTEX *****/
pthread_mutex_t *mutex_sendInfo;

/***** SIGNALS *****/
pthread_cond_t *ts_sendInfo;

/***** Objects *****/
CRFCom *m_rf;
			
class CThreadsAnimal
{
	public:
		CThreadsAnimal();
		~CThreadsAnimal();
		void run();
		
   private:
   		
   
   		/***** QUEUES *****/
			mqd_t mq_GPS;
			mqd_t mq_rf;
			
   		/***** PTHREADS *****/
			pthread_t t_RFComSender;
			pthread_t t_RFComReceiver;
			pthread_t t_shock;
			pthread_t t_batTemp;
			pthread_t t_processinInfo;
			pthread_t t_GPS;
			pthread_attr_t setAttr(int prio);
   		
			/***** PTHREADS BEHAVIOUR *****/
			static void * pv_RFComSenderHandler(void *threadid);
			static void * pv_RFComReceiverHandler(void *threadid);
			static void * pv_shockHandler(void *threadid);
			static void * pv_batTempHandler(void *threadid);
			static void * pv_processinInfoHandler(void *threadid);
			static void * pv_GPSHandler(void *threadid);
   		
   		
};
#endif /*THREADSANIMAL_H_*/
