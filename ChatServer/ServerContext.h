#pragma once
#include "TcpSocket.h"
#include "SessionManager.h"
#include "Room.h"
#include <set>

class ServerContext {

public:
	ServerContext();
	~ServerContext();

	BOOL Init(int port);
	BOOL Accept(TcpSocket& client_sock, FD_SET& rset);
	BOOL Run();
	void Close();

private:
	TcpSocket listenSock_;
	SessionManager* sessMgr_;

};