#include "Endpoint.h"
#include "TcpSocket.h"
#include <ws2tcpip.h>

EndPoint::EndPoint()
{
	ZeroMemory(&_serveraddr, sizeof(_serveraddr));
}

EndPoint::EndPoint(const char * ip, int port, IPType type)
{
	ZeroMemory(&_serveraddr, sizeof(_serveraddr));

	_IpType = type;
	if (type == IPType::IPv4) {
		auto pAddr = (SOCKADDR_IN*)&_serveraddr;

		pAddr->sin_family = AF_INET;
		pAddr->sin_addr.s_addr = htonl(INADDR_ANY);// inet_addr(ip);
		pAddr->sin_port = htons(port);
	}
	else {
		_serveraddr.sin6_family = AF_INET6;
		int len = sizeof(_serveraddr);
		WSAStringToAddressA((LPSTR)ip, AF_INET6, NULL,
			(SOCKADDR*)&_serveraddr, &len);
		_serveraddr.sin6_port = htons(port);
	}
}

EndPoint::EndPoint(int ip, int port)
{
	ZeroMemory(&_serveraddr, sizeof(_serveraddr));
	_IpType = IPType::IPv4;
	auto pAddr = (SOCKADDR_IN*)&_serveraddr;

	pAddr->sin_family = AF_INET;
	pAddr->sin_addr.s_addr = htonl(ip);
	pAddr->sin_port = htons(port);
}

int EndPoint::GetPort()
{
	if (_IpType == IPType::IPv4) {
		auto pAddr = (SOCKADDR_IN*)&_serveraddr;

		return ntohs(pAddr->sin_port);
	}
	else {
		return ntohs(_serveraddr.sin6_port);

	}
}

string EndPoint::GetAddrStr()
{
	if (_IpType == IPType::IPv4) 
	{
		auto pAddr = (SOCKADDR_IN*)&_serveraddr;
		return std::string(inet_ntoa(pAddr->sin_addr));
	}
	else 
	{
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToStringA((SOCKADDR*)&_serveraddr, sizeof(_serveraddr), NULL, ipaddr, &ipaddrlen);

		return std::string(ipaddr);
	}
}

int EndPoint::GetAddrSize() 
{
	if (_IpType == IPType::IPv4)
		return sizeof(SOCKADDR_IN);
	else 
		return sizeof(SOCKADDR_IN6);
}

void EndPoint::SetIPTypeFromAddrSize(int Addrsize) {
	if (Addrsize == sizeof(SOCKADDR_IN)) {
		_IpType = IPType::IPv4;
	}
	else {
		_IpType = IPType::IPv6;
	}
}

