/*
	threadsfield.h - Library to handle the field p_threads on Airfences project
	Created by Ricardo Roriz and Pedro Costa, 12 December 2016.
	Last Update by Ricardo Roriz, 3 February 2017
*/

/*****************************************************************************
	Class to handle all the threads of the Field Module
	Members:
		p_thread t_RFComSender -> Thread to Send Information by RadioFrequency
		p_thread t_RFComReceiver -> Thread to Receive Information By RadioFrequency
		p_thread t_WIFIComSender -> Thread to Send Information to the database
		p_thread t_WIFIComReceiver -> Thread to Receive Information to the database
		p_thread t_processAnimalInfo -> Thread to process Animal Requests,
															select the next Animal to Request and Handle Errors
	Functions:
	void run() -> run all the threads
	pthread_attr_t setAttr(int prio) -> Set the priority of Threads
		input: int prio -> priority
		output: pthread_attr_t with the right priority

*******************************************************************************/

#ifndef THREADSFIELD_H_
#define THREADSFIELD_H_

#include <iostream>
#include <pthread.h>
#include <mqueue.h>

#include "defines.h"
#include "NRF905.h"
#include "tcpcom.h"
#include "field.h"

/*
	Linked list to handle animals Regist and send to database
*/
struct SanimalRegist
{
	int id;
	int zone;
	int bat;
	double coorX;
	double coorY;
	float temp;
	SanimalRegist *pointer;
};

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

			pthread_attr_t setAttr(int prio);

			/***** PTHREADS BEHAVIOUR *****/
			static void * pv_RFComSenderConfHandler(void *threadid);
			static void * pv_RFComSenderHandler(void *threadid);
			static void * pv_RFComReceiverHandler(void *threadid);
			static void * pv_WIFIComSenderHandler(void *threadid);
			static void * pv_WIFIComReceiverHandler(void *threadid);
			static void * pv_processAnimalInfoHandler(void *threadid);


};
#endif /*THREADSFIELD_H_*/
