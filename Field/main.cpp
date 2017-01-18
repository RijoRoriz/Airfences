#include "tcpcom.h"


int main()
{
	CTcpCom a;
	char b[3];
	b[0]='o';
	b[1]='k';
	b[2]='\0';
	a.TcpComOpen();
	//a.TcpComReceive(b,2);
	a.TcpComTransmite(b, 3);
	printf("%c %c",b[0], b[1]);
	a.TcpComClose();
}
