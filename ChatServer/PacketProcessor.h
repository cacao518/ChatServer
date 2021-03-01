#pragma once
#include "RoomManager.h"
#include "SessionManager.h"
#include <functional>
#include <stdlib.h>

/*
	클라이언트로 부터 받은 패킷에 대해 핸들러 함수를 실행하는 클래스
*/

class PacketProceessor {

	typedef set<string> CommnadString;
public:
	static PacketProceessor* GetInstance();

	PacketProceessor();
	BOOL PacketProcess(Session* sess, const char* data);

	BOOL GotLogin(Session* sess, const char* data);
	BOOL GotHelp(Session* sess);
	BOOL GotExit(Session* sess);
	BOOL GotShowRoom(Session* sess);
	BOOL GotShowUser(Session* sess);
	BOOL GotShowRoomInfo(Session* sess, const char* data);
	BOOL GotShowUserInfo(Session* sess, const char* data);
	BOOL GotWhisper(Session* sess, const char* data);
	BOOL GotMakeRoom(Session* sess, const char* data);
	BOOL GotJoinRoom(Session* sess, const char* data);
	BOOL GotKick(Session* sess, const char* data);
	BOOL GotUnrealCheck(Session* sess, const char* data);

	void Chat(Session* sess);
	void SendWarningMessage(Session* sess, WarningKind warningKind);

public:
	using PacketHandler = function<BOOL(Session*, const char* data)>;
	unordered_map<PacketKind, PacketHandler> _packetHandleMap;		/// 핸들러 함수 집합 (key:패킷종류, value:함수)

private:
	static PacketProceessor* instance;
	vector<string> _command;
	//set<string> _command;	/// 명령어 string 집합
	RoomManager*	_roomMgr;	/// 룸 매니저
	SessionManager* _sessMgr;	/// 세션 매니저
};