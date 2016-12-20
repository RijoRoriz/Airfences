#ifndef NRF905_H_
#define NRF905_H_
#include "defines.h"

class CRFCom
{
	private:
	
		/***** Queues *****/
		mqd_t mq_GPS;
		mqd_t mq_rf;
				
		int m_cState;
		int i_SetIdle();
		int i_SetTx();
		int i_SetRx();
		unsigned char m_AddT[4];
		void RFComSetPayload(unsigned char *Payload);
		void RFComSetAddT(unsigned char *TxAddress);
		static pthread_mutex_t mutex_mode;
		
	public:
		CRFCom();
		~CRFCom();
		
		void RFComSetAddR(unsigned char *RxAddress);
		void RFComSender(unsigned char *TxAddress, unsigned char* Payload);
		void RFComPrintConf();
		void RFComPrintTAddr();
		void RFComPrintTPaylo();
		void RFComPrintRPaylo();
		void RFComReceiver();
};

#endif /*NRF905_H_*/
