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
public:
	BOOL AddSession(SOCKET sock);
	void RemoveSession(Session* client);

public:
	set<Session*>& GetClients() { return clients_; };

private:
	set<Session*> clients_;
};