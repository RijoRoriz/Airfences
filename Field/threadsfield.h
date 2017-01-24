#ifndef THREADSFIELD_H_
#define THREADSFIELD_H_

#include <iostream>
#include <pthread.h>
#include <mqueue.h>

#include "defines.h"
#include "NRF905.h"
#include "tcpcom.h"
#include "field.h"

class CThreadsField
{
	public:
        CThreadsField();
        ~CThreadsField();
        void run();
   private:
   		/***** PTHREADS *****/
			pthread_t t_RFComSender;
			pthread_t t_RFComReceiver;
			pthread_t t_WIFIComSender;
			pthread_t t_WIFIComReceiver;
			pthread_t t_processAnimalInfo;

			//static void pv_timerHandler(int sig, siginfo_t *si, void *uc);
			pthread_attr_t setAttr(int prio);
			//void pv_initTimer();

			/***** PTHREADS BEHAVIOUR *****/
			static void * pv_RFComSenderConfHandler(void *threadid);
			static void * pv_RFComSenderHandler(void *threadid);
			static void * pv_RFComReceiverHandler(void *threadid);
			static void * pv_WIFIComSenderHandler(void *threadid);
			static void * pv_WIFIComReceiverHandler(void *threadid);
			static void * pv_processAnimalInfoHandler(void *threadid);


};
#endif /*THREADSFIELD_H_*/
