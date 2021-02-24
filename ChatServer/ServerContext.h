#pragma once
#include "TcpSocket.h"
#include "SessionManager.h"
#include "PacketProcessor.h"
#include "RoomManager.h"
#include "Room.h"
#include <set>

class PacketProcessor;

class ServerContext {

public:
	ServerContext();
	~ServerContext();

	BOOL Init(int port);
	BOOL Accept(TcpSocket& client_sock, FD_SET& rset);
	BOOL Run();
	void Close();

private:
	TcpSocket		_listenSock;

	SessionManager* _sessMgr;
	RoomManager*	_roomMgr;

};