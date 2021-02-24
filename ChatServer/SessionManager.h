#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <set>
#include "TcpSocket.h"
#include "config.h"

class Session;

class SessionManager {
	static SessionManager* instance;
public:
	static SessionManager* GetInstance();

public:
	Session*		AddSession(SOCKET sock);
	void			RemoveSession(Session* client);

public:
	set<Session*>&	GetClients() { return _clients; };
	int				GetNewCode() { return _code++; }

private:
	int				_code = 0;
	set<Session*>	_clients;
};