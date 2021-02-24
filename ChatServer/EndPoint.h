#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>

#include "config.h"

using namespace std;

class EndPoint {

public:
	EndPoint();
	EndPoint(const char *ip, int port, IPType type);
	EndPoint(int ip, int port);


	int				GetPort();
	string			GetAddrStr();
	int				GetAddrSize();

	SOCKADDR*		GetAddr() { return (SOCKADDR*)&_serveraddr; }
	IPType			GetIpType() { return _IpType; };

	void			SetIPTypeFromAddrSize(int Addrsize);

private:
	SOCKADDR_IN6	_serveraddr;
	IPType			_IpType;

};