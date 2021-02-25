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

	// �̺κ��� ���� �Ľ� ó�� ����
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
	_packetHandleMap[PacketKind::Help] = [this](Session* sess, const char* data) { GotHelp(sess); };
	_packetHandleMap[PacketKind::Exit] = [this](Session* sess, const char* data) { GotExit(sess); };
	_packetHandleMap[PacketKind::ShowRoom] = [this](Session* sess, const char* data) { GotShowRoom(sess); };
	_packetHandleMap[PacketKind::ShowUser] = [this](Session* sess, const char* data) { GotShowUser(sess); };
	_packetHandleMap[PacketKind::ShowRoomInfo] = [this](Session* sess, const char* data) { GotShowRoomInfo(sess, data); };
	_packetHandleMap[PacketKind::ShowUserInfo] = [this](Session* sess, const char* data) { GotShowUserInfo(sess, data); };
	_packetHandleMap[PacketKind::Whisper] = [this](Session* sess, const char* data) { GotWhisper(sess, data); };
	_packetHandleMap[PacketKind::JoinRoom] = [this](Session* sess, const char* data) { GotJoinRoom(sess, data); };
	_packetHandleMap[PacketKind::MakeRoom] = [this](Session* sess, const char* data) { GotMakeRoom(sess, data); };
}

BOOL PacketProceessor::PacketProcess(Session* sess, const char* data)
{
	if (sess == nullptr) return FALSE;

	PacketKind pkKind;
	string str(data);

	// Ŭ��� ���� ���� data�� ��ɾ �ɸ����� üũ
	for (int i = 0; i < (int)PacketKind::End; i++)
	{
		// (1) ���� ���� �Ǿտ� Ŀ�ǵ尡 ���� ���
		if (str.find(_command[ i ] ) == 0)
		{
			// (2) Ŀ�ǵ� + ������ �и�
			auto range = _command[ i ].size();
			str.erase(0, range);
			str.erase(str.size() - 2, str.size());
			pkKind = (PacketKind)i;

			// (3) Ŀ�ǵ�(��Ŷ)�� �ش��ϴ� �Լ� ����
			_packetHandleMap[pkKind](sess, str.c_str());
			return TRUE;
		}
	}
	
	// ��ɾ �ɸ��� �ʴ´ٸ� �׳� �Ϲ� ä������ �Ѹ���
	Chat(sess);

	return TRUE;
}

/// �α��� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// �α����� �Ǿ������� ��� �Ұ���
	if (sess->GetIsLogin() == true) return;

	string message = "=========================================================\r\n		�α��εǾ����ϴ�.\r\n\r\n(���� : /help   ������ : /exit)\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewSessID(), data } );		// �α����� ���� ���� ���� ( ������ȣ, ���̵� )
	sess->SetCurRoom(_roomMgr->GetRooms()[0]);								// ���� �� �κ�� ����
	_roomMgr->GetRooms()[0]->EnterRoom(sess);								// �κ�� ���� (�κ� �ε��� = 0)

	sess->SetIsLogin(true);
}

void PacketProceessor::GotHelp(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	string message = "=========================================================\r\n";
	message.append("		*	����		*		\r\n");
	message.append("=========================================================\r\n");
	message.append("/help			: ����\r\n");
	message.append("/all user		: ��� ���� ����\r\n");
	message.append("/all room		: ��� �� ����\r\n");
	message.append("/r [���ȣ]		: �� ���� ����\r\n");
	message.append("/i [���̵�]		: ���� ���� ����\r\n");
	message.append("/w [���̵�] [�޽���]	: �ӼӸ�\r\n");
	message.append("/make [������]		: �� ����\r\n");
	message.append("/join [���ȣ]		: �� ����\r\n");
	message.append("/exit			: ������\r\n");
	message.append("=========================================================\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());
}

void PacketProceessor::GotExit(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	// ���� ���� �κ��� ���
	if (sess->GetCurRoom()->GetRoomInfo()._isLobby == true)
	{
		string message = "=========================================================\r\n		������ �����߽��ϴ�.\r\n=========================================================\r\n";
		sess->GetTcpSock().Send(message.c_str());
		_sessMgr->RemoveSession(sess);							// ���� ����
	}
	else // �Ϲ� �� ������ (�κ�� ����)
	{
		string message = "=========================================================\r\n		��ȭ���� �������ϴ�.\r\n=========================================================\r\n";
		sess->GetTcpSock().Send(message.c_str());

		sess->GetCurRoom()->LeaveRoom(sess);					// ���� �� ������
		sess->SetCurRoom(_roomMgr->GetRooms()[0]);				// ���� �� �κ�� ����
		_roomMgr->GetRooms()[0]->EnterRoom(sess);				// �κ�� ���� (�κ� �ε��� = 0)
	}
}

void PacketProceessor::GotShowRoom(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	_roomMgr->ShowRoomList(sess);
}

void PacketProceessor::GotShowUser(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	_sessMgr->ShowUserList(sess);
}

void PacketProceessor::GotShowRoomInfo(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	_roomMgr->ShowRoomInfo(sess, (UINT)atoi(data));
}

void PacketProceessor::GotShowUserInfo(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	_sessMgr->ShowUserInfo(sess, (UINT)atoi(data));
}

void PacketProceessor::GotWhisper(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;
}

void PacketProceessor::GotMakeRoom(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	string name(data);
	Room* room = _roomMgr->AddRoom(sess, ROOM_USER_MAX, name); // �游���
	if (room == nullptr) return;

	string message = "=========================================================\r\n		��ȭ���� �����Ǿ����ϴ�.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->GetCurRoom()->LeaveRoom(sess);					// ���� �� ������
	sess->SetCurRoom(room);									// ���ο� ������ ����
	room->EnterRoom(sess);									// ���ο� �� ����
}

void PacketProceessor::GotJoinRoom(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return;

	// �� �˻�
	auto roomIter = _roomMgr->GetRooms().find(atoi(data));
	if (roomIter == _roomMgr->GetRooms().end()) return;		// �� ������ ��� 

	// ���� ���� �ִ� ���̸� ���
	auto room = (*roomIter).second;
	if (room == sess->GetCurRoom()) return;					

	string message = "=========================================================\r\n		��ȭ�濡 �����߽��ϴ�.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->GetCurRoom()->LeaveRoom(sess);					// ���� �� ������
	sess->SetCurRoom(room);									// ���ο� ������ ����
	(*roomIter).second->EnterRoom(sess);					// ���ο� �� ����
}


/// ��� �ƴ� �Ϲ� ä�� �Լ�(�濡 �ִ� ������� ��ü���� �޼����� �Ѹ���)
void PacketProceessor::Chat(Session * sess)
{
	// ���� ������ ����
	if (sess->GetCurRoom() == nullptr) return;

	int addrlen;
	SOCKADDR_IN clientaddr;
	string toClientBuf;

	// ���� ���ڿ� �ٹ̱�
	toClientBuf.append("\r\n");
	string str = "[" + sess->GetPlayerInfo().name + "] : " + sess->GetTcpSock().GetTotalBuf();
	toClientBuf.append( str );

	// ������ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(sess->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[%s : %d]", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	printf(" %s", str.c_str());

	// �濡 �ִ� Ŭ��鿡�� ������
	toClientBuf.append("�Է�> ");
	sess->GetCurRoom()->SendData(sess, toClientBuf.c_str());

	// ä���� ���� Ŭ���̾�Ʈ > Ŀ�� �ٽ� ǥ��
	sess->GetTcpSock().Send("�Է�> ");

	// ���� ����
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();
}
