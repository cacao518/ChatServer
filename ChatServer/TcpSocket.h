#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <vector>
#include "EndPoint.h"
#include "config.h"

class EndPoint;
class Session;
using namespace std;

class TcpSocket {

public:
	TcpSocket();
	TcpSocket(SocketType socketType, IPType ipType);
	~TcpSocket();

	void Bind(const EndPoint& endpoint);
	void Listen();
	int Accept(TcpSocket& acceptedSocket);
	Error Send(const char* data, int length);
	Error Receive();
	void Close();

public:
	TcpSocket*		GetThis() { return this; };
	EndPoint&		GetEndPoint() { return endpoint_; };
	SOCKET&			GetSocket() { return sock_; }
	char&			GetBuf() { return buf_; }
	vector<char>&	GetTotalBuf() { return totalBuf_; }
	int				GetRecvBytes() { return recvbytes_; }
	int				GetSocketType() { return socketType_; }

	string			GetIPAddress(); // 아이피 주소(숫자)를 반환
	int				GetPort(); // 포트번호(숫자)를 반환

	void			SetBuf(char buf) { this->buf_ = buf; }
	void			SetSocket(SOCKET sock) { this->sock_ = sock; }
	void			SetRecvBytes(int recvbytes) { this->recvbytes_ = recvbytes; }
	void			SetSession(Session* sess) { this->parent_ = sess; };

private:
	SOCKET			sock_;
	EndPoint		endpoint_;
	Session*		parent_;

	char			buf_;
	vector<char>	totalBuf_;
	int				recvbytes_;
	int				socketType_;
};