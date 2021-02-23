#pragma once
#include <unordered_map>
#include "Session.h"
#include "config.h"

class Room {

public:
	Room();
	void EnterRoom(Session* sess);
	void ExitRoom(Session* sess);
	void SendData(Session* sess, const char* data);
	void SendAllToRoomMembers(const char* data);

	int getNewCode() { return codeAccumulator++; }

private:
	int codeAccumulator = 0;
	string roomName_;
	int memberNumMax_;
	unordered_map<Session*, PlayerInfo> members_;
};