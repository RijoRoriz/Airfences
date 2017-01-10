#include <tcpcom.h>

CTcpCom()::CTcpCom()
{
	memset(&addr, 0, sizeof(addr));
	host = gethostbyname("172.16.49.01"); /* select IP adress */
	port = 5000;
	addr.sin_family = AF_INET; /* select internet protocol */
	addr.sin_port = port;
	addr.sin_addr.s_addr = * (long*)(host->h_addr_list[0]); /* set the addr */
	sd = socket(PF_INET,SOCK_STREAM,0);
	if(sd<0) {
	perror("socket not created"); abort();
	}	
}

bool CTcpCom()::TcpOpen()
{
	if(connect(sd,(struct sockaddr*)&addr,sizeof(addr))==0) {
	m_bconnected=TRUE;
	return TRUE;
	}
	else
	{
	 printf("socket error");
	 m_bconnected=FALSE;
	 return FALSE;
	}
}

bool CTcpCom()::TcpClose()
{
	
}

void CTcpCom()::TcpComReceive()
