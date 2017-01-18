#ifndef THREADSFIELD_H_
#define THREADSFIELD_H_

class CThreadsField
{
	public:
        CThreadsField();
        ~CThreadsField();
        void run();
   private:
   		/***** PTHREADS *****/
			pthread_t m_RFComSenderConf;
			pthread_t m_RFComSender;
			pthread_t m_RFComReceiver;
			pthread_t m_WIFISender;
			pthread_t m_WIFIReceiver;
			pthread_t m_processAnimalInfo;

			/***** PTHREADS BEHAVIOUR *****/
			static void * m_RFComSenderConfHandler();
			static void * m_RFComSenderHandler();
			static void * m_RFComReceiverHandler();
			static void * m_WIFISenderHandler();
			static void * m_WIFIReceiverHandler();
			static void * m_processAnimalInfoHandler();
			static void m_timerHandler(int);
			void SetupThread(int prio,pthread_attr_t *pthread_attr,struct sched_param *pthread_param);

}
#endif /*THREADSFIELD_H_*/
