#include "defines.h"
#include "NRF905.h"


int main()
{
	//CThreadsAnimal animal;
	//animal.run();
	mqd_t mq_GPS;
	mqd_t mq_rf;
	mq_rf = mq_open(MQRFCOM, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
	mq_GPS = mq_open(MQGPS, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
	CRFCom teste;
	teste.RFComPrintConf();
	unsigned char array[32]={1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//teste.RFComSender(NULL,array);
	teste.RFComPrintRPaylo();
	teste.RFComPrintTPaylo();
	teste.RFComReceiver();
	//teste.RFComPrintRPaylo();
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComSender(NULL,array);
	teste.RFComPrintTPaylo();

	mq_close(mq_GPS);
	mq_close(mq_rf);
	mq_unlink(MQGPS);
	mq_unlink(MQRFCOM);

}
