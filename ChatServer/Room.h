#pragma once
#include <set>
#include "Session.h"
#include "config.h"

class Room {

public:
	Room(int roomCode);
	Room(Session * master, int roomCode, int max, string name);

	void EnterRoom(Session* sess);
	void LeaveRoom(Session* sess);
	void SendData(Session* sess, const char* data);
	void SendAllToRoomMembers(const char* data);

	RoomInfo		GetRoomInfo() { return _info; }
	set<Session*>&	GetMembers() { return _members; };

private:
	RoomInfo		_info;
	Session*		_master;
	set<Session*>	_members;
};