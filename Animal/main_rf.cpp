#include "defines.h"
#include "NRF905.h"

using namespace std;

int main()
{
	unsigned char array[33];

	CRFCom teste;
	teste.RFComPrintConf();

	//teste.RFComSender(NULL,array);
	teste.RFComPrintRPaylo();
	teste.RFComPrintTPaylo();
	teste.RFComPrintTAddr();
	teste.RFComReceiver(array);

cout << array << endl;

	// teste.RFComSender(NULL,array);

	teste.RFComPrintTPaylo();
}
