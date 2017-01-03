#include "NRF905.h"

 
CRFCom::CRFCom(){
	
	wiringPiSetup();
	
	/***** RFConfiguration *****/
	unsigned char RFConf[11]=
	{
		NRF905_WRITE_CONF,
		0x76,                   //CH_NO,868.4MHZ 76
      0x0C,                   //output power 10db, resend disable, Current Normal operation
      0x44,                   //4-byte address
      0x20,0x20,              //receive or send data length 32 bytes
      0xCC,0xCC,0xCC,0xCC,    //receiving address
      0x58,                   //CRC enable,8bit CRC,external clock disable,16MHZ Oscillator
	};
   
	//Init nRF905
	pinMode(CSN, OUTPUT);
	pinMode(DR, INPUT);	// Init DR for input
	pinMode(AM, INPUT);// Init AM for input
	pinMode(CD, INPUT);// Init CD for input
	pinMode(PWR_RF, OUTPUT);
	digitalWrite(PWR_RF, HIGH);// nRF905 power on
	pinMode(CE, OUTPUT);
	digitalWrite(CE, LOW);// Set nRF905 in standby mode
	pinMode(TX_EN, OUTPUT);
	digitalWrite(TX_EN, HIGH);// set radio in Rx mode
	
	if(wiringPiSPISetup(0, 16000000) == -1){
	  printf("Could not initialise SPI\n");
	}
	else{
		/***** send write configuration command ******/
		wiringPiSPIDataRW(0, RFConf, 11);
		//i_SetRx();
		
	}
	m_AddT[0]=0xCC;
	m_AddT[1]=0xCC;
	m_AddT[2]=0xCC;
	m_AddT[3]=0xCC;
	
	unsigned char command[5];
	command[0]= NRF905_WRITE_TX_ADDR;
	command[1]= m_AddT[0];
	command[2]= m_AddT[1];
	command[3]= m_AddT[2];
	command[4]= m_AddT[3];
	wiringPiSPIDataRW(0, command, 5);
	digitalWrite(CE,0);
	i_SetRx();
}

void CRFCom::RFComSetAddR(unsigned char *RxAddress)
{
	
   uint8_t RFConf[11]=
	{
		NRF905_WRITE_CONF,
		0x00,                  //CH_NO,868.4MHZ
      0x0C,                   //output power 10db, resend disable, Current Normal 											operation
      0x44,                   //4-byte address
      0x20,0x20,              //receive or send data length 32 bytes
      0xCC,0xCC,0xCC,0xCC,    //receiving address
      0x58,                   //CRC enable,8bit CRC,external clock disable,16MHZ 									Oscillator
	};
	
	RFConf[6]=RxAddress[0];
	RFConf[7]=RxAddress[1];
	RFConf[8]=RxAddress[2];
	RFConf[9]=RxAddress[3];				// Spi enable for write a spi command
	
	/***** send write configuration command ******/
	wiringPiSPIDataRW(0, RFConf, 11);
	//i_SetRx();
	
}

void CRFCom::RFComSetAddT(unsigned char *TxAddress)
{
	m_AddT[0]=TxAddress[0];
	m_AddT[1]=TxAddress[1];
	m_AddT[2]=TxAddress[2];
	m_AddT[3]=TxAddress[3];
	
	unsigned char command[5];
	command[0]= NRF905_WRITE_TX_ADDR;
	command[1]= TxAddress[0];
	command[2]= TxAddress[1];
	command[3]= TxAddress[2];
	command[4]= TxAddress[3];
	wiringPiSPIDataRW(0, command, 5);
	delay(1);
}

CRFCom::~CRFCom(){

}

int CRFCom::i_SetTx()
{
	if(m_cState==0)
		{
		digitalWrite(CE, 0);
		digitalWrite(TX_EN, HIGH);
		m_cState=1;
		delay(1);
   }
   return 0;

}

int CRFCom::i_SetRx()
{
	if(m_cState==1)
	{
		digitalWrite(CE, 1);
		digitalWrite(TX_EN, 0);
		m_cState=0;
		delay(1);
   }
   return 0;
}

void CRFCom::RFComSender(unsigned char *TxAddress, unsigned char *Payload)
{
		
		unsigned int sender;
		unsigned char msgcontentGPS[MQGPSLEN];
		/***** Open Queue *****/
		/*int mq_error = mq_receive(mq_GPS, (char*)msgcontentGPS, MQGPSLEN ,&sender);
		if (mq_error == -1) {
		     perror("In mq_receive()");
		     exit(1);
		}*/
		
		/***** prepare data to send *****/
		i_SetTx(); //Set transmit mod
		if(TxAddress!=NULL)
		{
			this->RFComSetAddT(TxAddress); 
			printf("TxAdd not null");
		} 
		if(Payload!=NULL)
		{
		 this->RFComSetPayload(Payload);
		 printf("Payload not null");
		}
		else
		{
			//RFComSetPayload(msgcontentGPS);
		}
		digitalWrite(CE,1);
		delay(1);
		digitalWrite(CE,0);
}
	
void CRFCom::RFComReceiver(unsigned char * returned)
{
		i_SetRx();
		while(digitalRead(DR)==0);
   	returned[0]= NRF905_READ_RX_PAYLOAD;		
		wiringPiSPIDataRW(0, returned, 33);	
		digitalWrite(CE, 1);
		digitalWrite(CE, 0);
}

void CRFCom::RFComPrintConf()
{
	unsigned char configurationread [11];
   configurationread[0]=NRF905_READ_CONF;
	//wiringPiSPIDataRW(0, &command, 1);
	wiringPiSPIDataRW(0, configurationread, 11);	
	for(int i=1;i<11;i++)
	{
		printf("Conf[%d]:%x ", i,configurationread[i]);
	}
	printf("\n");	
}

void CRFCom::RFComPrintTAddr()
{
	unsigned char configurationread [5];
   configurationread[0]=NRF905_READ_TX_ADDR;
	wiringPiSPIDataRW(0, configurationread, 5);	
	for(int i=1;i<5;i++)
	{
		printf("Addr:%x ", configurationread[i]);
	}
	printf("\n");
}


void CRFCom::RFComPrintTPaylo()
{
	unsigned char configurationread [33];
   configurationread[0]=NRF905_READ_TX_PAYLO;		
	wiringPiSPIDataRW(0, configurationread, 33);	
	printf("TPayload:");
	for(int i=1;i<33;i++)
	{
		printf("%x ", configurationread[i]);
	}
	printf("\n");
}

void CRFCom::RFComPrintRPaylo()
{
	unsigned char configurationread [33];
   configurationread[0]= NRF905_READ_RX_PAYLOAD;		
	wiringPiSPIDataRW(0, configurationread, 33);	
	printf("RPayload:");
	for(int i=1;i<33;i++)
	{
		printf("%x ", configurationread[i]);
	}
	
	printf("\n");
}

void CRFCom::RFComSetPayload(unsigned char *Payload)
{
	unsigned char payload[MQRFCOMLEN+1];
	payload[0]= NRF905_WRITE_TX_PAYLOAD;
	for(int i=1;i<33;i++)
	{
		payload[i]=Payload[i-1];
	}
	wiringPiSPIDataRW(0, payload, NRF905_PAYLOAD_SIZE+1);
	delay(1);	
}

