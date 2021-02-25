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
	BOOL	Send(const char* data);
	BOOL	Receive();
	void	Close();

public:
	TcpSocket*		GetThis() { return this; };
	EndPoint&		GetEndPoint() { return _endpoint; };
	SOCKET&			GetSocket() { return _sock; }
	char&			GetBuf() { return _buf; }
	string&			GetTotalBuf() { return _totalBuf; }
	int				GetSocketType() const { return _socketType; }

	string			GetIPAddress();		// ������ �ּ�(����)�� ��ȯ
	int				GetPort();			// ��Ʈ��ȣ(����)�� ��ȯ

	void			SetBuf(const char& buf) { this->_buf = buf; }
	void			SetSocket(const SOCKET sock) { this->_sock = sock; }
	void			SetSession(Session* sess) { this->_parent = sess; };

private:
	SOCKET			_sock;
	EndPoint		_endpoint;			// ���Ͼ�巹��, ��Ʈ
	Session*		_parent;			// �� ������ ������ �ִ� session

	char			_buf;				// char(�ѱ���) ����
	string			_totalBuf;			// buf�� ��Ƶ� string ����
	int				_socketType;		// ���� Ÿ��(TCP, UDP)
};