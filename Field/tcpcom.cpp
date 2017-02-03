#include "tcpcom.h"

using namespace std;

CTcpCom::CTcpCom()
{
	//memset(&addr, 0, sizeof(addr));
	host = gethostbyname("10.42.0.1"); /* select IP adress */
	port = htons(80);
	addr.sin_family = AF_INET; /* select internet protocol */
	addr.sin_port = port;
	addr.sin_addr.s_addr = * (long*)(host->h_addr_list[0]); /* set the addr */
	}


void CTcpCom::TcpComPrintInfo()
{
	cout << "Host:" << host << '\n';
	cout << "Port:" << port << '\n';
}

CTcpCom::~CTcpCom()
{
	TcpComClose();
}

bool CTcpCom::TcpComOpen()
{
	sd = socket(AF_INET,SOCK_STREAM,0);
	if(sd<0) {
	perror("socket not created");
	return false;
	}
	else{
			int valor=0;
			if(connect(sd,(const sockaddr*)&addr,sizeof(addr))>-1) {
			m_bconnected=true;
			return true;
			}
			else
			{
			 cout << valor <<"socket error";
			 m_bconnected=false;
			 return false;
			}
		}
}

bool CTcpCom::TcpComClose()
{
	shutdown(sd,SHUT_RDWR);
	m_bconnected=false;
}

int CTcpCom::TcpComReceive(char * returned, int length)
{
	if(m_bconnected){
	return read(sd,returned,length);
	}
	else return -1;
}

int CTcpCom::TcpComTransmite(char * info, int length)
{
	if(m_bconnected){
	return write(sd,info,length);
	}
	else return -1;
}
