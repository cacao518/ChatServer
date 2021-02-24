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
	// data ���� 
	PacketKind pkKind;
	string str(data);
	for (int i = 0; i < (int)PacketKind::End; i++)
	{
		if (str.find(_command[ i ] ) == 0) // ���� ���� �Ǿտ� Ŀ�ǵ尡 ���� ���
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

// �α��� ��ɾ�(��Ŷ) �޾��� ��
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// �α����ϸ� ó���� ������ �κ�� ����

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );		// �α����� ���� ���� ����

	sess->SetParent(_roomMgr->GetRooms()[0]);		// ���� �θ�(������ ��) ����

	_roomMgr->GetRooms()[0]->EnterRoom(sess);		// �κ�� ���� (�κ� �ε��� = 0)
}
