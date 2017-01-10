#ifndef TCPCOM_H_
#define TCPCOM_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <defines.h>

#define TCPCOMLENGTH 32

class CTcpCom
{
	private:
		struct hostent*  host;
		struct sockaddr_in addr;
		int sd, port;
		bool m_bconnected;
	
	public:
		CTcpCom();
		~CTcpCom();
		bool TcpComOpen();
		bool TcpComClose();
		int TcpComReceive(char * returned);
		int TcpComTransmite(char * info);
};

#endif /*TCPCOM_H*/
	
