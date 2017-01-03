#ifndef ANIMAL_H_
#define ANIMAL_H_

#include <unistd.h>
#include <signal.h>

#include "threadsanimal.h"

using namespace std;

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
