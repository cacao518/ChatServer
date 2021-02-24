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

class TcpSocket {

public:
	TcpSocket();
	TcpSocket(SocketType socketType, IPType ipType);
	~TcpSocket();

	void	Bind(const EndPoint& endpoint);
	void	Listen();
	int		Accept(TcpSocket& acceptedSocket);
	BOOL	Send(const char* data, int length);
	BOOL	Receive();
	void	Close();

public:
	TcpSocket*		GetThis() { return this; };
	EndPoint&		GetEndPoint() { return _endpoint; };
	SOCKET&			GetSocket() { return _sock; }
	char&			GetBuf() { return _buf; }
	vector<char>&	GetTotalBuf() { return _totalBuf; }
	int				GetRecvBytes() const { return _recvbytes; }
	int				GetSocketType() const { return _socketType; }

	string			GetIPAddress(); // 아이피 주소(숫자)를 반환
	int				GetPort(); // 포트번호(숫자)를 반환

	void			SetBuf(const char buf) { this->_buf = buf; }
	void			SetSocket(const SOCKET sock) { this->_sock = sock; }
	void			SetRecvBytes(const int recvbytes) { this->_recvbytes = recvbytes; }
	void			SetSession(Session* sess) { this->_parent = sess; };

private:
	SOCKET			_sock;
	EndPoint		_endpoint;
	Session*		_parent;

	char			_buf;
	vector<char>	_totalBuf;
	int				_recvbytes;
	int				_socketType;
};