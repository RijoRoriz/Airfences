/*
	NRF905.h - Library to handle the Radio Frequency module on Airfences project
	Created by Ricardo Roriz and Pedro Costa, 12 December 2016.
	Last Update by Ricardo Roriz, 4 February 2017
*/

/*****************************************************************************
	Class to handle RF comunication between animals and field
	Members:
	bool m_cState -> state of the Module
	true -> transmit mode
	false -> receve mode
	unsigned char m_AddT[4] -> the address to transmit

	Functions:
	void RFComSetPayload(unsigned char *Payload) -> Set the payload to transmit
		input: unsigned char *Payload (32 bytes) -> Payload

	void RFComSetAddT(unsigned char *TxAddress) -> Set the Addr to transmit
		input: unsigned char *TxAddress (4 bytes) -> Address

	void RFComSetAddR(unsigned char *RxAddress) -> Set the Address to receve
		input: unsigned char *RxAddress (4 bytes)-> Address

	void RFComSender(unsigned char *TxAddress, unsigned char* Payload) -> send the
											payload to the address
		input: unsigned char *TxAddress (4 bytes) -> Address
					 unsigned char *Payload (32 bytes) -> Payload

	void RFComPrintConf() -> print the configuration
	void RFComPrintTAddr() -> print the transmit address
	void RFComPrintTPaylo() -> print the transmit payload
	void RFComPrintRPaylo() -> print the receve payload

	bool RFComReceiver(unsigned char * returned) -> receve the payload to returned char
		input: unsigned char * returned -> payload receved

*******************************************************************************/

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

		void RFComSetAddR(unsigned char *RxAddress);
		void RFComSender(unsigned char *TxAddress, unsigned char* Payload);
		void RFComPrintConf();
		void RFComPrintTAddr();
		void RFComPrintTPaylo();
		void RFComPrintRPaylo();
		bool RFComReceiver(unsigned char * returned);
};

#endif /*NRF905_H_*/
