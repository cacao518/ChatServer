#include "TcpSocket.h"

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
	if (sock_ == INVALID_SOCKET) err_quit("socket()");
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
	//ZeroMemory(&serveraddr_, sizeof(serveraddr_));
	//serveraddr_.sin_family = IpType_;
	//serveraddr_.sin_addr.s_addr = inet_addr(ip);// htonl(INADDR_ANY);
	//serveraddr_.sin_port = htons(port);

	//int retval;
	//retval = bind(sock_, (SOCKADDR*)&serveraddr_, sizeof(serveraddr_));
	//if (retval == SOCKET_ERROR) err_quit("bind()");

	endpoint_ = endpoint;
	auto retval = bind(sock_, endpoint_.GetAddr(), endpoint_.GetAddrSize());
	if (retval == SOCKET_ERROR)  err_quit("bind()");
}


void TcpSocket::Listen()
{
	// listen()
	int retval;
	retval = listen(sock_, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(sock_, FIONBIO, &on);
	if (retval == SOCKET_ERROR) err_display("ioctlsocket()");
}

int TcpSocket::Accept(TcpSocket & acceptedSocket)
{
	int addrLen = endpoint_.GetAddrSize();

	auto sock = accept(sock_, acceptedSocket.endpoint_.GetAddr(), &addrLen);
	acceptedSocket.sock_ = sock;
	acceptedSocket.endpoint_.SetIPTypeFromAddrSize(addrLen);


	return sock;
}

int TcpSocket::Send(const char * data, int length)
{
	return 0;
}

int TcpSocket::Receive()
{
	return 0;
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




// 소켓 함수 오류 출력 후 종료
void TcpSocket::err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void TcpSocket::err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}