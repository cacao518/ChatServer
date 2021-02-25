#pragma once
#include "TcpSocket.h"
#include "Room.h"

class Room;

/*	Ŭ���̾�Ʈ�� ������ �ϴ� Ŭ����      */

class Session {

public:	
	Session(TcpSocket socket);
	~Session();

	TcpSocket&	GetTcpSock() { return _socket; };
	SOCKET&		GetSock() { return _socket.GetSocket(); };
	BOOL		Run(FD_SET& rset, FD_SET& wset);				// FD_SET�� ������ ��� �ִٸ� �б�/���⸦ ����

	////////////////////////////////////

	bool		GetIsLogin() { return _isLogin; };
	PlayerInfo	GetPlayerInfo() { return _info; }
	Room*		GetCurRoom() { return _curRoom; }
	void		SetIsLogin(const bool& isLogin) { _isLogin = isLogin; };
	void		SetIsExit(const bool& isExit) { _isExit = isExit; };
	void		SetPlayerInfo(const PlayerInfo& info);
	void		SetCurRoom(Room* parent) { _curRoom = parent; }

private:
	TcpSocket	_socket;					// Tcp ����
	PlayerInfo	_info;						// ���� ����(���̵�, �̸�)
	Room*		_curRoom;					// ���� �� 

	bool		_isLogin = false;			// �α��� ����
	bool		_isExit = false;			// ���α׷� ���� ����
};