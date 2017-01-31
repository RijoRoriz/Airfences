#include "defines.h"
#include "NRF905.h"

using namespace std;

int main( int argc, char * argv [] )
{
	unsigned char message[33];
	unsigned char addrT[4] = {0xAA, 0xAA, 0x00, 0x01};
	unsigned char addrR[4] = {0xFF, 0xFF, 0x00, 0x01};
	uint16_t idAnimal = 1;
	uint16_t idField = 1;
	float lat1 = 41.501333;
	float long1 = -8.349984;
	float lat2 = 41.503463;
	float long2 = -8.347560;

	CRFCom *p_rf;

	// cout << "ARGC: " << argc << endl;
	// cout << "ARGV: " << argv[1] << endl;

	p_rf = new CRFCom();
	p_rf->RFComSetAddR(addrR);
	// p_rf->RFComSetAddT(addrT);

	if(argc == 2) {
		char *command = argv[1];
		// cout << *command;
		memset(message, '\0', 33);

		memcpy(&message[0], &idField, 2);
    memcpy(&message[2], &idAnimal, 2);

		switch (*command) {
			case 'R': // RESET
			case 'r':
			cout << "RESET COMMAND" << endl;
			message[4] = 'R';
			message[5] = 1; // TEMP
			message[6] = 1; // BAT
			message[7] = 1; // GPS
			message[8] = 1; // RF
			message[9] = 1; // STATE
			break;

			case 'I': // REQUEST INFO
			case 'i':
			cout << "REQUEST COMMAND" << endl;
			message[4] = 'I';
			message[5] = 1; // TEMP
			message[6] = 1; // BAT
			message[7] = 1; // GPS
			message[8] = 1; // RF
			message[9] = 1; // STATE
			break;

			case 'N': // FIRST CONF
			case 'n':
			cout << "FIRST CONF COMMAND" << endl;
			memcpy(&message[2], "0", 2); // ID DE FABRICA
			message[4] = 'N';
			memcpy(&message[5], &idAnimal, 2); // ID ANIMAL
			memcpy(&message[7], &lat1, 4);   // LAT 1
			memcpy(&message[11], &lat2, 4);  // LAT 2
			memcpy(&message[15], &long1, 4); // LONG 1
			memcpy(&message[19], &long2, 4); // LONG 2
			break;

			case 'C': // NEW CONF
			case 'c':
			cout << "NEW CONF COMMAND" << endl;
			message[4] = 'C';
			memcpy(&message[5], &lat1, 4);   // LAT 1
			memcpy(&message[9], &lat2, 4);  // LAT 2
			memcpy(&message[13], &long1, 4); // LONG 1
			memcpy(&message[17], &long2, 4); // LONG 2
			break;

			default:
			exit(1);
			break;
		}

		p_rf->RFComSender(addrT, message);

		cout << "Message: ";

		for(int i=0; i < 33; i++)
		{
			printf("%X", message[i]);
		}
		cout << endl;
	}
	else {
		exit(1);
	}
	// p_rf->RFComPrintConf();
	// p_rf->RFComPrintTAddr();
}


// int main()
// {
// 	CRFCom teste;
//
// 	// unsigned char payload[33];
// 	//
// 	// uint16_t idAnimal, idField;
// 	// char command;
// 	// float temp, bat;
// 	// float lat1, lat2, long1, long2;
// 	//
// 	// //memset(payload, '\0', 33);
// 	//
// 	// idAnimal = 2584;
// 	// idField = 3;
// 	// command = 'C';
// 	// temp = 27.53;
// 	// bat = 11.02;
// 	//
// 	// //payload[0] = (idAnimal & 0xFF00) >> 8;
// 	// //payload[1] = (idAnimal & 0x00FF);
// 	// //memcpy(&payload[0], &idAnimal, 2); //É preciso fazer swap troca os bytes o memcpy
// 	//
// 	// payload[2] = (idField & 0xFF00) >> 8;
// 	// payload[3] = (idField & 0x00FF);
// 	//
// 	// payload[4] = command;
// 	//
// 	// //Temperature
// 	// uint8_t inteiro = static_cast<int>(temp);
// 	// uint8_t decimal = (temp - inteiro) * 100;
// 	//
// 	// payload[5] = inteiro;
// 	// payload[6] = decimal;
// 	//
// 	// //Battery
// 	// uint8_t inteiro1 = static_cast<int>(bat);
// 	// uint8_t decimal1 = (bat - inteiro1) * 100;
// 	//
// 	// payload[7] = inteiro1;
// 	// payload[8] = decimal1;
// 	//
// 	// //Zone
// 	// uint8_t zone = 1; //1, 2, 3, 4
// 	// payload[9] = zone;
// 	//
// 	// //Alerts ? numero de choques?
// 	// uint8_t alert = 5;
// 	// payload[10] = alert;
// 	//
// 	// lat1 = -84.25415;
// 	// lat2 = -71.24685;
// 	// long1 = 140.54127;
// 	// long2 = -23.41578;
// 	//
// 	// int8_t graus_int1 = static_cast<int>(lat1);
// 	// uint32_t graus_dec1 = abs(lat1 - graus_int1) * 100000;
// 	//
// 	// int8_t graus_int2 = static_cast<int>(lat2);
// 	//
// 	//
// 	// //GPS lat1 - 5
// 	// //graus -90º a 90º
// 	// payload[11] = graus_int1;
// 	//
// 	// //parte decimal
// 	// payload[12] = (graus_dec1 & 0xFF000000) >> 24;
// 	// payload[13] = (graus_dec1 & 0x00FF0000) >> 16;
// 	// payload[14] = (graus_dec1 & 0x0000FF00) >> 8;
// 	// payload[15] = (graus_dec1 & 0x000000FF);
// 	//
// 	// //memcpy(&payload[16], &graus_dec1, 4); //É preciso fazer swap dos bytes
// 	//
// 	// int degrees_lat1 = payload[11];
// 	// int degrees_lat1_dec = (payload[12] << 24 | payload[13] << 16 | payload[14] << 8 | payload[15]);
// 	//
// 	// float aux = (abs(degrees_lat1) + static_cast<float>(degrees_lat1_dec) / 100000);
// 	// float total_degrees_lat1;
// 	//
// 	// if (degrees_lat1 < 0) {
// 	// 	total_degrees_lat1 = aux * -1;
// 	// }
// 	// else {
// 	// 	total_degrees_lat1 = aux;
// 	// }
//
// 	unsigned char array[33]={1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//
// 	teste.RFComPrintConf();
//
// 	teste.RFComSender(NULL,array);
//
// 	//teste.RFComSender(NULL,payload);
// 	//teste.RFComPrintTAddr();
// 	//teste.RFComReceiver(array);
// 	//teste.RFComPrintTPaylo();
// 	//teste.RFComReceiver(array);
// 	return 0;
//
// }
