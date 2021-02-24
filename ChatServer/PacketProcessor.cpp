#include "PacketProcessor.h"
#include "RoomManager.h"
#include "Room.h"
#include "Session.h"

PacketProceessor::PacketProceessor()
{
	_roomMgr = RoomManager::GetInstance();
	_sessMgr = SessionManager::GetInstance();
}


// 로그인 명령어(패킷) 받았을 때
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// 로그인하면 처음에 무조건 로비로 입장

	// 로그인한 세션 정보 셋팅
	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );
	
	// 로비방 입장 (로비 인덱스 = 0)
	_roomMgr->GetRooms()[0]->EnterRoom(sess);
}
