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

	void Chat(Session* sess);
	void SendWarningMessage(Session* sess);

public:
	using PacketHandler = function<BOOL(Session*, const char* data)>;
	unordered_map<PacketKind, PacketHandler> _packetHandleMap;		/// �ڵ鷯 �Լ� ���� (key:��Ŷ����, value:�Լ�)

private:
	vector<string> _command;	/// ��ɾ� string ����
	RoomManager*	_roomMgr;	/// �� �Ŵ���
	SessionManager* _sessMgr;	/// ���� �Ŵ���
};