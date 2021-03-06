#ifndef NRF905_H_
#define NRF905_H_
#include "defines.h"

class CRFCom
{
	private:
		bool m_cState; //0 - RX mode   1 - TX mode

		void setTx();
		void setRx();
		unsigned char m_AddT[4];

		void RFComSetPayload(unsigned char *Payload);
		void RFComSetAddT(unsigned char *TxAddress);

	public:
		CRFCom();
		~CRFCom();

		void RFComReset();

		void RFComSetAddR(unsigned char *RxAddress);
		void RFComSender(unsigned char *TxAddress, unsigned char* Payload);
		void RFComPrintConf();
		void RFComPrintTAddr();
		void RFComPrintTPaylo();
		void RFComPrintRPaylo();
		void RFComReceiver(unsigned char * returned);
};

#endif /*NRF905_H_*/
