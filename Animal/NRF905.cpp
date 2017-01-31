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
		0xAA,0xAA,0x00,0x01,    //receiving address
		0x58,                   //CRC enable,8bit CRC,external clock disable,16MHZ Oscillator
	};

	//Init nRF905
	pinMode(CSN, OUTPUT);
	digitalWrite(CSN, HIGH); //SPI disable
	pinMode(DR, INPUT);	// Init DR for input
	pinMode(AM, INPUT);// Init AM for input
	pinMode(CD, INPUT);// Init CD for input
	pinMode(PWR_RF, OUTPUT);
	digitalWrite(PWR_RF, HIGH);// nRF905 power on
	pinMode(CE, OUTPUT);
	digitalWrite(CE, LOW);// Set nRF905 in standby mode
	pinMode(TX_EN, OUTPUT);
	digitalWrite(TX_EN, LOW);// set radio in Rx mode

	if(wiringPiSPISetup(0, 16000000) == -1){
	  printf("Could not initialise SPI\n");
	}
	else{
		/***** send write configuration command ******/
		digitalWrite(CSN, LOW); //SPI enable
		wiringPiSPIDataRW(0, RFConf, 11);
		digitalWrite(CSN, HIGH); //SPI disable
	}

	m_cState = 0;

	m_AddT[0]=0xFF;
	m_AddT[1]=0xFF;
	m_AddT[2]=0x00;
	m_AddT[3]=0x01;

	unsigned char command[5];
	command[0]= NRF905_WRITE_TX_ADDR;
	command[1]= m_AddT[0];
	command[2]= m_AddT[1];
	command[3]= m_AddT[2];
	command[4]= m_AddT[3];

	digitalWrite(CSN, LOW); //Enable SPI
	wiringPiSPIDataRW(0, command, 5);
	digitalWrite(CSN, HIGH); //Disable SPI
}

CRFCom::~CRFCom(){

}

void CRFCom::RFComSetAddR(unsigned char *RxAddress)
{
	digitalWrite(CE,LOW);

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

	RFConf[6]=RxAddress[0];
	RFConf[7]=RxAddress[1];
	RFConf[8]=RxAddress[2];
	RFConf[9]=RxAddress[3];				// Spi enable for write a spi command

	/***** send write configuration command ******/
	digitalWrite(CSN, LOW); //Enable SPI
	wiringPiSPIDataRW(0, RFConf, 11);
	digitalWrite(CSN, HIGH); //Disable SPI
	digitalWrite(CE,HIGH);

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
	digitalWrite(CSN, LOW);
	wiringPiSPIDataRW(0, command, 5);
	digitalWrite(CSN, HIGH); //Disable SPI
	delay(1);
}

void CRFCom :: setTx()
{
	//if(m_cState==0)
	{
		digitalWrite(CE, LOW);
		digitalWrite(TX_EN, HIGH);
		m_cState=1;
		delay(1);
	}
}

void CRFCom :: setRx()
{
	//if(m_cState==1)
	{
		digitalWrite(CE, HIGH);
		digitalWrite(TX_EN, LOW);
		m_cState=0;
		delayMicroseconds(800);
   }
}

void CRFCom::RFComReset()
{
	digitalWrite(PWR_RF, LOW);// nRF905 power off
	delay(3);
	digitalWrite(PWR_RF, HIGH);// nRF905 power on
}

void CRFCom::RFComSender(unsigned char *TxAddress, unsigned char *Payload)
{
		/***** prepare data to send *****/
		setTx(); //Set transmit mod
		delay(1);
		if(TxAddress!=NULL)
		{
			this->RFComSetAddT(TxAddress);
			// printf("TxAdd not null");
		}
		if(Payload!=NULL)
		{
		 this->RFComSetPayload(Payload);
		 //RFComPrintTPaylo();
		 //printf("Payload not null");
		}
		else
		{
			//RFComSetPayload(msgcontentGPS);
		}
		digitalWrite(CE, HIGH);
		delay(1);
		digitalWrite(CE, LOW);
}

void CRFCom::RFComReceiver(unsigned char * returned)
{
		setRx();
		while(digitalRead(DR)==0);
		delay(1);
		digitalWrite(CE, LOW);
		digitalWrite(CSN, LOW); //Enable SPI
		delay(1);
   	returned[0]= NRF905_READ_RX_PAYLOAD;
		wiringPiSPIDataRW(0, returned, 33);
		digitalWrite(CSN, HIGH); //Disable SPI
		delay(1);
		//digitalWrite(CE, HIGH); //RX remains ON
		digitalWrite(CE, LOW); //Radio enters in standby
		delay(1);

		for(int i=0; i < 32; i++) {
			returned[i] = returned[i+1];
		}
}

void CRFCom::RFComPrintConf()
{
	unsigned char configurationread [11];
	digitalWrite(CSN, LOW); //Enable SPI
   configurationread[0]=NRF905_READ_CONF;
	wiringPiSPIDataRW(0, configurationread, 11);
	digitalWrite(CSN, HIGH); //Disable SPI
	for(int i=1;i<11;i++)
	{
		printf("Conf[%d]:%x ", i,configurationread[i]);
	}
	printf("\n");
}

void CRFCom::RFComPrintTAddr()
{
	unsigned char configurationread [5];
	digitalWrite(CSN, LOW); //Enable SPI
   configurationread[0]=NRF905_READ_TX_ADDR;
	wiringPiSPIDataRW(0, configurationread, 5);
	digitalWrite(CSN, HIGH); //Disable SPI
	for(int i=1;i<5;i++)
	{
		printf("Addr:%x ", configurationread[i]);
	}
	printf("\n");
}

void CRFCom::RFComPrintTPaylo()
{
	unsigned char configurationread [33];
	digitalWrite(CSN, LOW); //Enable SPI
   configurationread[0]=NRF905_READ_TX_PAYLO;
	wiringPiSPIDataRW(0, configurationread, 33);
	digitalWrite(CSN, HIGH); //Disable SPI
	printf("TPayload:");
	for(int i=1;i<33;i++)
	{
		printf("%X ", configurationread[i]);
	}
	printf("\n");
}

void CRFCom::RFComPrintRPaylo()
{
	unsigned char configurationread [33];
	digitalWrite(CSN, LOW); //Enable SPI
   configurationread[0]= NRF905_READ_RX_PAYLOAD;
	wiringPiSPIDataRW(0, configurationread, 33);
	digitalWrite(CSN, HIGH); //Disable SPI
	printf("RPayload:");
	for(int i=1;i<33;i++)
	{
		printf("%X ", configurationread[i]);
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
	digitalWrite(CSN, LOW); //Enable SPI
	wiringPiSPIDataRW(0, payload, NRF905_PAYLOAD_SIZE+1);
	digitalWrite(CSN, HIGH); //Disable SPI
}
