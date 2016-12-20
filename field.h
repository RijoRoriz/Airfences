#ifndef FIELD_H_
#define FIELD_H_

#include <mqueue.h> 
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "threadsfield.h"
#include "fieldmap.h"



struct Sanimal
{
	int id;
	int zone;
};

struct SanimalRegist
{
	int id;
	int zone;
	int bat;
	bool coorX;
	bool coorY;
	float temp;
	
};


class Cfield
{
	private:
		struct SSquare m_fieldzone;
		struct Sanimal *mL_greenList;
		struct Sanimal *mL_yellowList;
		struct Sanimal *mL_redList;
		struct SanimalRegist *mL_sendInfoList;
		int mi_setRWFlag(bool);	
		/***** Timer *****/
		struct itimerval timer;
		
	public: 
		/***** Message Queue *****/
		mqd_t mq_wifi;
		mqd_t mq_rf;
		
		/***** Mutex *****/
		pthread_mutex_t mutex_newConfig;
		pthread_mutex_t mutex_newInfo;
		
		/***** Condition Variables (SIGNALS) *****/
		pthread_cond_t s_newConfig;
		pthread_cond_t s_newInfo;
		Cfield();
		~Cfield();
	
};

#endif /*FIELD_H_*/
