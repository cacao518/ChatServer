#include "TcpSocket.h"
#include "SessionManager.h"

TcpSocket::TcpSocket()
{
}

TcpSocket::TcpSocket(SocketType socketType, IPType ipType)
{
	// 윈속 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// socket() 
	int type, af;
	if (ipType == IPType::IPv4) 
		af = AF_INET;
	else 
		af = AF_INET6;

	if (socketType == SocketType::TCP) 
		type = SOCK_STREAM;
	else 
		type = SOCK_DGRAM;
	
	_sock = socket(af, type, 0);
	if (_sock == INVALID_SOCKET) ErrorUtil::err_quit("socket()");
}

TcpSocket::~TcpSocket()
{
	if (_sock != 0) {
		Close();
	}
}

void TcpSocket::Bind(const EndPoint& endpoint)
{
	// bind()
	_endpoint = endpoint;
	auto retval = bind(_sock, _endpoint.GetAddr(), _endpoint.GetAddrSize());
	if (retval == SOCKET_ERROR) ErrorUtil::err_quit("bind()");
}


void TcpSocket::Listen()
{
	// listen()
	int retval = listen(_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrorUtil::err_quit("listen()");

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) ErrorUtil::err_display("ioctlsocket()");
}

int TcpSocket::Accept(TcpSocket & acceptedSocket)
{
	int addrLen = _endpoint.GetAddrSize();

	auto sock = accept(_sock, acceptedSocket._endpoint.GetAddr(), &addrLen);
	acceptedSocket._sock = sock;
	acceptedSocket._endpoint.SetIPTypeFromAddrSize(addrLen);

	return sock;
}

BOOL TcpSocket::Send(const char * data)
{
	int retval = send(_sock, data, strlen(data), 0);

	if (retval == SOCKET_ERROR) 
	{
		ErrorUtil::err_display("send()");
		SessionManager::GetInstance()->RemoveSession(_parent);
		return FALSE;
	}
	return TRUE;
}

BOOL TcpSocket::Receive()
{
	int retval = recv(_sock, &(_buf), sizeof(char), 0); // 한글자만 받아

	if (retval == SOCKET_ERROR) {
		ErrorUtil::err_display("recv()");
		SessionManager::GetInstance()->RemoveSession(_parent);
		return FALSE;
	}
	else if (retval == 0) {
		SessionManager::GetInstance()->RemoveSession(_parent);
		return FALSE;
	}
	return TRUE;
}

void TcpSocket::Close()
{
	closesocket(_sock);
	_sock = 0;
}

string TcpSocket::GetIPAddress()
{
	return _endpoint.GetAddrStr();
}

int TcpSocket::GetPort()
{
	return _endpoint.GetPort();
}



