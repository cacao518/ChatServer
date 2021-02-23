#pragma once

#include "TcpSocket.h"


class Session {

public:	
	Session(TcpSocket& socket);
	~Session();

	TcpSocket&	GetTcpSock() { return socket_; };
	SOCKET&		GetSock() { return socket_.GetSocket(); };

	void Start();

private:
	TcpSocket socket_;


};