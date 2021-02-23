#pragma once

#include "TcpSocket.h"


class Session {

public:	
	Session(TcpSocket socket);
	~Session();

	TcpSocket&	GetTcpSock() { return socket_; };
	SOCKET&		GetSock() { return socket_.GetSocket(); };

	Error Run(FD_SET& rset, FD_SET& wset);

private:
	TcpSocket socket_;


};