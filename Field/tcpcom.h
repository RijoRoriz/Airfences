#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <defines.h>

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
		void TcpComReceive(char * returned);
		void TcpComTransmite(char * info);
	
