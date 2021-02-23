#pragma once
#include "TcpSocket.h"
#include "SessionManager.h"

class ServerContext {

public:
	ServerContext(SessionManager& sessMgr);
	~ServerContext();

	Error Init(int port);
	Error Accept(SOCKET& client_sock);
	Error Run();

	void Close();


private:
	TcpSocket listenSock_;
	SessionManager& sessMgr_;

};