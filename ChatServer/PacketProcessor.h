#pragma once
#include "RoomManager.h"
#include "SessionManager.h"
#include <functional>

class PacketProceessor {

	static PacketProceessor* instance;
public:
	static PacketProceessor* GetInstance();

	PacketProceessor();

	BOOL PacketProcess(Session* sess, const char* data);
	void GotLogin(Session* sess, const char* data);
	void Chat(Session* sess);

public:
	using PacketHandler = std::function<void(Session*, const char* data)>;
	unordered_map<PacketKind, PacketHandler> _packetHandleMap;

private:
	vector<string> _command;

	RoomManager*	_roomMgr;
	SessionManager* _sessMgr;
};