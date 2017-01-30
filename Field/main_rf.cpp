#include "defines.h"
#include "NRF905.h"

using namespace std;

int main()
{
	unsigned char array[33];

	memset(array, '\0', 33);

	CRFCom teste;
	teste.RFComPrintConf();
	//
	// //teste.RFComSender(NULL,array);
	// teste.RFComPrintRPaylo();
	// teste.RFComPrintTPaylo();
	teste.RFComPrintTAddr();
	while(1)
	{
		delay(1);
		if(teste.RFComReceiver(array)){
			cout << "RECEIVED: ";

			for(int i=1; i < 33; i++)
			{
				printf("%x", array[i] );
			}
		}
		else {
			cout << "TIMEOUT" << endl;
		}
		// teste.RFComReceiver(array);


	}

//cout << hex << array << endl;

	// teste.RFComSender(NULL,array);

	//teste.RFComPrintTPaylo();
}

// #include "defines.h"
// #include "NRF905.h"
//
//
// int main()
// {
// 	CRFCom teste;
// 	teste.RFComPrintConf();
// 	unsigned char array[33]={1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// 	//teste.RFComSender(NULL,array);
// 	teste.RFComPrintTAddr();
// 	teste.RFComReceiver(array);
// 	//teste.RFComPrintTPaylo();
// 	//teste.RFComReceiver(array);
// }
