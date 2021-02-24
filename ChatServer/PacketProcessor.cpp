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
	_command.push_back("/exit");
	_command.push_back("/all user");
	_command.push_back("/all room");
	_command.push_back("/r ");
	_command.push_back("/i ");
	_command.push_back("/w ");
	_command.push_back("/make ");
	_command.push_back("/join ");

	_packetHandleMap[PacketKind::Login] = [this](Session* sess, const char* data) { GotLogin(sess, data); };
}

BOOL PacketProceessor::PacketProcess(Session* sess, const char* data)
{
	if (sess == nullptr) return FALSE;

	PacketKind pkKind;
	string str(data);

	// Ŭ��� ���� ���� data�� ��ɾ �ɸ����� üũ
	for (int i = 0; i < (int)PacketKind::End - 1; i++)
	{
		if (str.find(_command[ i ] ) == 0) // ���� ���� �Ǿտ� Ŀ�ǵ尡 ���� ���
		{
			auto range = _command[ i ].size();
			str.erase(0, range);
			str.erase(str.size() - 2, str.size());
			pkKind = (PacketKind)i;

			_packetHandleMap[pkKind](sess, str.c_str());
			return TRUE;
		}
	}
	
	// ��ɾ �ɸ��� �ʴ´ٸ� �׳� �Ϲ� ä������ �Ѹ���
	Chat(sess);

	return TRUE;
}

// �α��� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// �α����ϸ� ó���� ������ �κ�� ����

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );		// �α����� ���� ���� ����

	sess->SetParent(_roomMgr->GetRooms()[0]);		// ���� �θ�(������ ��) ����

	_roomMgr->GetRooms()[0]->EnterRoom(sess);		// �κ�� ���� (�κ� �ε��� = 0)
}


// ��� �ƴ� �Ϲ� ä�� �Լ�(�濡 �ִ� ������� ��ü���� �޼����� �Ѹ���)
void PacketProceessor::Chat(Session * sess)
{
	// ���� ������ ����
	if (sess->GetParent() == nullptr) return;

	int addrlen;
	SOCKADDR_IN clientaddr;

	string retBuf;

	// ������ ������
	retBuf.append("\r\n");
	
	string str = "[" + sess->GetPlayerInfo().name + "] : " + sess->GetTcpSock().GetTotalBuf();
	retBuf.append( str );

	// retBuf ������ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(sess->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf.c_str());

	// �濡 �ִ� Ŭ��鿡�� ������
	retBuf.append(">");
	sess->GetParent()->SendData(sess, retBuf.c_str());

	// ä���� ���� Ŭ���̾�Ʈ > Ŀ�� �ٽ� ǥ��
	sess->GetTcpSock().Send(">", strlen(">"));

	// ���� ����
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();
}
