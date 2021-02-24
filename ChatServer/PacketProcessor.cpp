#include "PacketProcessor.h"
#include "RoomManager.h"
#include "Room.h"
#include "Session.h"

PacketProceessor* PacketProceessor::instance = nullptr;

PacketProceessor * PacketProceessor::GetInstance()
{
	if (instance == NULL)
		instance = new PacketProceessor();

	return instance;
}

PacketProceessor::PacketProceessor()
{
	_roomMgr = RoomManager::GetInstance();
	_sessMgr = SessionManager::GetInstance();

	_command.push_back("/login ");
	_command.push_back("/help");
	_command.push_back("/user");
	_command.push_back("/room");
	_command.push_back("/r ");
	_command.push_back("/i ");
	_command.push_back("/w ");
	_command.push_back("/make ");
	_command.push_back("/join ");

	_packetHandleMap[PacketKind::Login] = [this](Session* sess, const char* data) { GotLogin(sess, data); };
}

BOOL PacketProceessor::PacketProcess(Session* sess, const char* data)
{
	// data 에서 
	PacketKind pkKind;
	string str(data);
	for (int i = 0; i < (int)PacketKind::End; i++)
	{
		if (str.find(_command[ i ] ) == 0) // 받은 문자 맨앞에 커맨드가 있을 경우
		{
			auto range = _command[ i ].size();
			str.erase(0, range);
			pkKind = (PacketKind)i;

			_packetHandleMap[pkKind](sess, str.c_str());
			
			return TRUE;
		}
	}
	
	return FALSE;
}

// 로그인 명령어(패킷) 받았을 때
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// 로그인하면 처음에 무조건 로비로 입장

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );		// 로그인한 세션 정보 셋팅

	sess->SetParent(_roomMgr->GetRooms()[0]);		// 세션 부모(입장한 방) 셋팅

	_roomMgr->GetRooms()[0]->EnterRoom(sess);		// 로비방 입장 (로비 인덱스 = 0)
}
