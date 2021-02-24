#include "PacketProcessor.h"
#include "RoomManager.h"
#include "Room.h"
#include "Session.h"

PacketProceessor::PacketProceessor()
{
	_roomMgr = RoomManager::GetInstance();
	_sessMgr = SessionManager::GetInstance();
}


// �α��� ��ɾ�(��Ŷ) �޾��� ��
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// �α����ϸ� ó���� ������ �κ�� ����

	// �α����� ���� ���� ����
	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );
	
	// �κ�� ���� (�κ� �ε��� = 0)
	_roomMgr->GetRooms()[0]->EnterRoom(sess);
}
