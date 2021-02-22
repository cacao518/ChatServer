#include "Endpoint.h"
#include "TcpSocket.h"
#include <ws2tcpip.h>

EndPoint::EndPoint()
{
	ZeroMemory(&serveraddr_, sizeof(serveraddr_));
}

EndPoint::EndPoint(const char * ip, int port, IPType type)
{
	ZeroMemory(&serveraddr_, sizeof(serveraddr_));

	IpType_ = type;
	if (type == IPType::IPv4) {
		auto pAddr = (SOCKADDR_IN*)&serveraddr_;

		pAddr->sin_family = AF_INET;
		pAddr->sin_addr.s_addr = inet_addr(ip);// htonl(INADDR_ANY);
		pAddr->sin_port = htons(port);
	}
	else {
		serveraddr_.sin6_family = AF_INET6;
		int len = sizeof(serveraddr_);
		WSAStringToAddressA((LPSTR)ip, AF_INET6, NULL,
			(SOCKADDR*)&serveraddr_, &len);
		serveraddr_.sin6_port = htons(port);
	}
}

EndPoint::EndPoint(int ip, int port)
{
	ZeroMemory(&serveraddr_, sizeof(serveraddr_));
	IpType_ = IPType::IPv4;
	auto pAddr = (SOCKADDR_IN*)&serveraddr_;

	pAddr->sin_family = AF_INET;
	pAddr->sin_addr.s_addr = htonl(ip);
	pAddr->sin_port = htons(port);
}

int EndPoint::GetPort()
{
	if (IpType_ == IPType::IPv4) {
		auto pAddr = (SOCKADDR_IN*)&serveraddr_;

		return ntohs(pAddr->sin_port);
	}
	else {
		return ntohs(serveraddr_.sin6_port);

	}
}

string EndPoint::GetAddrStr()
{
	if (IpType_ == IPType::IPv4) 
	{
		auto pAddr = (SOCKADDR_IN*)&serveraddr_;
		return std::string(inet_ntoa(pAddr->sin_addr));
	}
	else 
	{
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToStringA((SOCKADDR*)&serveraddr_, sizeof(serveraddr_), NULL, ipaddr, &ipaddrlen);

		return std::string(ipaddr);
	}
}

int EndPoint::GetAddrSize() 
{
	if (IpType_ == IPType::IPv4)
		return sizeof(SOCKADDR_IN);
	else 
		return sizeof(SOCKADDR_IN6);
}

void EndPoint::SetIPTypeFromAddrSize(int Addrsize) {
	if (Addrsize == sizeof(SOCKADDR_IN)) {
		IpType_ = IPType::IPv4;
	}
	else {
		IpType_ = IPType::IPv6;
	}
}

