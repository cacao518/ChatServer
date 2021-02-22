#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <vector>
#include "EndPoint.h"
#include "config.h"

class EndPoint;

using namespace std;


class TcpSocket {

public:
	TcpSocket();
	TcpSocket(SocketType socketType, IPType ipType);
	~TcpSocket();

	void Bind(const EndPoint& endpoint);
	void Listen();
	int Accept(TcpSocket& acceptedSocket);
	int Send(const char* data, int length);
	int Receive();
	void Close();

public:
	TcpSocket*		GetThis() { return this; };
	EndPoint&		GetEndPoint() { return endpoint_; };
	SOCKET&			GetSocket() { return sock_; }
	char&			GetBuf() { return buf_; }
	vector<char>	GetTotalBuf() { return totalBuf_; }
	int				GetRecvBytes() { return recvbytes_; }
	int				GetSocketType() { return socketType_; }

	string			GetIPAddress(); // 아이피 주소(숫자)를 반환
	int				GetPort(); // 포트번호(숫자)를 반환

	void			SetBuf(char buf) { buf = buf_; }
	void			SetSocket(SOCKET& sock ) { sock = sock_; }
	void			SetRecvBytes(int recvbytes) { recvbytes = recvbytes_; }
		
public:
	// 오류 출력 함수
	void err_quit(const char* msg);
	void err_display(const char* msg);


private:
	SOCKET			sock_;
	EndPoint		endpoint_;

	char			buf_;
	vector<char>	totalBuf_;
	int				recvbytes_;
	int				socketType_;
};