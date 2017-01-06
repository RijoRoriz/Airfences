#include "defines.h"
#include "NRF905.h"


int main()
{
	CRFCom teste;
	teste.RFComPrintConf();
	unsigned char array[33]={1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//teste.RFComSender(NULL,array);
	teste.RFComPrintTAddr();
	teste.RFComReceiver(array);
	//teste.RFComPrintTPaylo();
	//teste.RFComReceiver(array);
}
