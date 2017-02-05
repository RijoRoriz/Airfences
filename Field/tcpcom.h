/*
	tcpcom.h - Library to handle the comunication with the database on Airfences project
	Created by Ricardo Roriz and Pedro Costa, 12 December 2016.
	Last Update by Ricardo Roriz, 3 February 2017
*/

/*****************************************************************************
	Class to handle WIFI comunication between database and field
	Members:
	struct hostent*  host -> host
	struct sockaddr_in addr -> address
	int sd, port -> port to connect and socket
	bool m_bconnected -> bool to check if the connection is OK

	Functions:
	bool TcpComOpen() -> open the socket and try connection with the host
		output: bool -> true if the connection is OK

	bool TcpComClose() -> close the socket and close the connection
		output: bool -> true if the connection is closed

	int TcpComReceive(char * returned, int length) -> read the socket
		input: char * returned -> buffer with the mensage returned
					 int length -> length of the buffer
		output: int -> returns the length of the message

	int TcpComTransmite(char * info, int length) -> write on socket
		input: char * info -> buffer with the mensage
					 int length -> length of the buffer
		output: int -> returns the length of the message

*******************************************************************************/
#ifndef TCPCOM_H_
#define TCPCOM_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include "defines.h"
#include <unistd.h>

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
		int TcpComReceive(char * returned, int length);
		int TcpComTransmite(char * info, int length);
		void TcpComPrintInfo();
};

#endif /*TCPCOM_H*/
