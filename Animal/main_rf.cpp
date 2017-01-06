#include "defines.h"
#include "NRF905.h"


int main()
{
	CRFCom teste;
	teste.RFComPrintConf();
	unsigned char array[32]={1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//teste.RFComSender(NULL,array);
	teste.RFComPrintRPaylo();
	teste.RFComPrintTPaylo();
	teste.RFComPrintTAddr();
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
}
