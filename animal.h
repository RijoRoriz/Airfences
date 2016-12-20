#ifndef ANIMAL_H_
#define ANIMAL_H_

#include <mqueue.h> 
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "gps.h"
#include "defines.h"
#include "fieldmap.h"
#include "leds.h"
#include "threadsanimal.h"
#define OUTZONE 4
#define REDZONE 3
#define YELLOWZONE 2
#define GREENZONE 1
#define MQSENSORS    "/mq_sensors"
#define MQGPSLEN     50
#define MQRFCOM   "/mq_rf"
#define MQRFCOMLEN    50

class CAnimal
{
	private:
		int mi_zone;
		int mi_id;
		int mi_temp;
		float mf_bat;
		Cgps *m_gps;
		Cleds *m_leds;	
		CRFCom *m_rf;
		CThreads *m_threads;
		int mi_setRWFlag(bool);
		
		/***** Message Queue *****/
		mqd_t mq_sensors;
		
		/***** Timer *****/
		struct itimerval timer;
		
		/***** Interrupt *****/
		struct sigaction interrupt_act;
		int mi_reconfTimer(int);
		
		
		
	public: 
		CAnimal();
		~CAnimal();
		
		/***** Mutex *****/
		pthread_mutex_t  mutex_endProcessing;
		pthread_mutex_t  mutex_GPSReady;
		pthread_mutex_t  mutex_sendInfo;
		
		/***** Condition Variables (SIGNALS) *****/
		pthread_cond_t  ts_sendInfo;
		pthread_cond_t  ts_endProcessing;
		pthread_cond_t  ts_GPSReady;
		static void v_timerHandler(int);
	
};

#endif /*ANIMAL_H_*/
