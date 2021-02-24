#pragma once

#include "TcpSocket.h"
#include "Room.h"

class Session {

public:	
	Session(TcpSocket socket);
	~Session();

	TcpSocket&	GetTcpSock() { return _socket; };
	SOCKET&		GetSock() { return _socket.GetSocket(); };
	BOOL		Run(FD_SET& rset, FD_SET& wset);

	////////////////////////////////////

	PlayerInfo	GetPlayerInfo() { return _info; };
	Room*		GetParent() { return _parent; };
	void		SetPlayerInfo(PlayerInfo info);
	void		SetParent(Room* parent) { _parent = parent; };

private:
	TcpSocket	_socket;
	PlayerInfo	_info;
	Room*		_parent;
};