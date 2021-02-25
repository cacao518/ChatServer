#pragma once
#include "RoomManager.h"
#include "SessionManager.h"
#include <functional>
#include <stdlib.h>

class PacketProceessor {

	static PacketProceessor* instance;
public:
	static PacketProceessor* GetInstance();

	PacketProceessor();
	BOOL PacketProcess(Session* sess, const char* data);

	void GotLogin(Session* sess, const char* data);
	void GotHelp(Session* sess);
	void GotExit(Session* sess);
	void GotShowRoom(Session* sess);
	void GotShowUser(Session* sess);
	void GotShowRoomInfo(Session* sess, const char* data);
	void GotShowUserInfo(Session* sess, const char* data);
	void GotWhisper(Session* sess, const char* data);
	void GotMakeRoom(Session* sess, const char* data);
	void GotJoinRoom(Session* sess, const char* data);

	void Chat(Session* sess);
	
public:
	using PacketHandler = std::function<void(Session*, const char* data)>;
	unordered_map<PacketKind, PacketHandler> _packetHandleMap;		/// 핸들러 함수 집합 (key:패킷종류, value:함수)

private:
	vector<string> _command;	/// 명령어 string 집합


	RoomManager*	_roomMgr;	/// 룸 매니저
	SessionManager* _sessMgr;	/// 세션 매니저
};