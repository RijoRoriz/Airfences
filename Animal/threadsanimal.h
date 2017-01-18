#ifndef THREADSANIMAL_H_
#define THREADSANIMAL_H_

#include <iostream>
#include <pthread.h>
#include <mqueue.h>

#include "defines.h"
#include "adc.h"
#include "animal.h"
#include "fieldmap.h"
#include "gps.h"
#include "leds.h"
#include "NRF905.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

using namespace std;

class CThreadsAnimal
{
public:
	CThreadsAnimal();
	~CThreadsAnimal();
	void run();

private:

	/***** PTHREADS *****/
	pthread_t t_shock;
	pthread_t t_batTemp;
	pthread_t t_gps;
	pthread_t t_processinInfo;
	pthread_t t_RFComSender;
	pthread_t t_RFComReceiver;

	pthread_attr_t setAttr(int prio);
	void pv_initTimer();
	static void pv_handleTimer(int sig, siginfo_t *si, void *uc);

	/***** PTHREADS BEHAVIOUR *****/
	static void * pv_RFComSenderHandler(void *threadid);
	static void * pv_RFComReceiverHandler(void *threadid);
	static void * pv_shockHandler(void *threadid);
	static void * pv_batTempHandler(void *threadid);
	static void * pv_processinInfoHandler(void *threadid);
	static void * pv_gpsHandler(void *threadid);

};

#endif /*THREADSANIMAL_H_*/
