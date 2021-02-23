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
	
	sock_ = socket(af, type, 0);
	if (sock_ == INVALID_SOCKET) ErrorUtil::err_quit("socket()");
}

TcpSocket::~TcpSocket()
{
	if (sock_ != 0) {
		Close();
	}
}

void TcpSocket::Bind(const EndPoint& endpoint)
{
	// bind()
	endpoint_ = endpoint;
	auto retval = bind(sock_, endpoint_.GetAddr(), endpoint_.GetAddrSize());
	if (retval == SOCKET_ERROR) ErrorUtil::err_quit("bind()");
}


void TcpSocket::Listen()
{
	// listen()
	int retval;
	retval = listen(sock_, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrorUtil::err_quit("listen()");

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(sock_, FIONBIO, &on);
	if (retval == SOCKET_ERROR) ErrorUtil::err_display("ioctlsocket()");
}

int TcpSocket::Accept(TcpSocket & acceptedSocket)
{
	int addrLen = endpoint_.GetAddrSize();

	auto sock = accept(sock_, acceptedSocket.endpoint_.GetAddr(), &addrLen);
	acceptedSocket.sock_ = sock;
	acceptedSocket.endpoint_.SetIPTypeFromAddrSize(addrLen);

	return sock;
}

Error TcpSocket::Send(const char * data, int length)
{
	int retval;
	retval = send(sock_, data, strlen(data), 0);

	if (retval == SOCKET_ERROR) 
	{
		ErrorUtil::err_display("send()");
		SessionManager::GetInstance()->RemoveSession(parent_);
		return Error::SEND_ERROR;
	}
	return Error::None;
}

Error TcpSocket::Receive()
{
	int retval;
	retval = recv(sock_, &(buf_), sizeof(char), 0); // 한글자만 받아

	if (retval == SOCKET_ERROR) {
		ErrorUtil::err_display("recv()");
		SessionManager::GetInstance()->RemoveSession(parent_);
		return Error::RECV_ERROR;
	}
	else if (retval == 0) {
		SessionManager::GetInstance()->RemoveSession(parent_);
		return Error::RECV_ERROR;
	}
	return Error::None;
}

void TcpSocket::Close()
{
	closesocket(sock_);
	sock_ = 0;
}

string TcpSocket::GetIPAddress()
{
	return  endpoint_.GetAddrStr();
}

int TcpSocket::GetPort()
{
	return endpoint_.GetPort();
}



