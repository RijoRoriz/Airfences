#include "tcpcom.h"

using namespace std;

CTcpCom::CTcpCom()
{
	//memset(&addr, 0, sizeof(addr));
	host = gethostbyname("10.42.0.1"); /* select IP adress */
	port = htons(5000);
	addr.sin_family = AF_INET; /* select internet protocol */
	addr.sin_port = port;
	addr.sin_addr.s_addr = * (long*)(host->h_addr_list[0]); /* set the addr */
	sd = socket(PF_INET,SOCK_STREAM,0);
	if(sd<0) {
	perror("socket not created");
	}
}

void CTcpCom::TcpComPrintInfo()
{
	cout << "Host:" << host << '\n';
	cout << "Port:" << port << '\n';
}

CTcpCom::~CTcpCom()
{
	if(m_bconnected==false) TcpComClose();
}

bool CTcpCom::TcpComOpen()
{
	int valor=0;
	if(connect(sd,(const sockaddr*)&addr,sizeof(addr))==0) {
	m_bconnected=true;
	return true;
	}
	else
	{
	 cout << valor <<"socket error";
	 m_bconnected=true;
	 return false;
	}
}

bool CTcpCom::TcpComClose()
{
	shutdown(sd,SHUT_RDWR);
	m_bconnected=false;
}

int CTcpCom::TcpComReceive(char * returned, int TCPCOMLENGTH)
{
	if(m_bconnected){
	recv(sd,returned,TCPCOMLENGTH,0);
	return 0;
	}
	else return -1;
}

int CTcpCom::TcpComTransmite(char * info, int TCPCOMLENGTH)
{
	if(m_bconnected){
	send(sd,info,TCPCOMLENGTH,0);
	return 0;
	}
	else return -1;
}
