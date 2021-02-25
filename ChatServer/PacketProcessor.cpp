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

/// ��Ŷ�� �´� �ڵ鷯 �Լ����� ����صд�.
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

	// ���ú� �ڵ鷯 �Լ� ���
	_packetHandleMap[PacketKind::Login] = [this](Session* sess, const char* data) { return GotLogin(sess, data); };
	_packetHandleMap[PacketKind::Help] = [this](Session* sess, const char* data) { return GotHelp(sess); };
	_packetHandleMap[PacketKind::Exit] = [this](Session* sess, const char* data) { return GotExit(sess); };
	_packetHandleMap[PacketKind::ShowRoom] = [this](Session* sess, const char* data) { return GotShowRoom(sess); };
	_packetHandleMap[PacketKind::ShowUser] = [this](Session* sess, const char* data) { return GotShowUser(sess); };
	_packetHandleMap[PacketKind::ShowRoomInfo] = [this](Session* sess, const char* data) { return GotShowRoomInfo(sess, data); };
	_packetHandleMap[PacketKind::ShowUserInfo] = [this](Session* sess, const char* data) { return GotShowUserInfo(sess, data); };
	_packetHandleMap[PacketKind::Whisper] = [this](Session* sess, const char* data) { return GotWhisper(sess, data); };
	_packetHandleMap[PacketKind::JoinRoom] = [this](Session* sess, const char* data) { return GotJoinRoom(sess, data); };
	_packetHandleMap[PacketKind::MakeRoom] = [this](Session* sess, const char* data) { return GotMakeRoom(sess, data); };
}

/// ������ ���ú긦 �ϰ� �� ������ �� �Լ��� �����Ͽ� �ڵ鷯�� �����Ѵ�.
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
			if(_packetHandleMap[pkKind](sess, str.c_str()) == FALSE) 
				SendWarningMessage(sess);

			return TRUE;
		}
	}
	
	// ��ɾ �ɸ��� �ʴ´ٸ� �׳� �Ϲ� ä������ �Ѹ���
	Chat(sess);

	return TRUE;
}

/// �α��� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// �α����� �Ǿ������� ��� �Ұ���
	if (sess->GetIsLogin() == true) return FALSE;

	string message = "=========================================================\r\n		�α��εǾ����ϴ�.\r\n\r\n(���� : /help   ������ : /exit)\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewSessID(), data } );		// �α����� ���� ���� ���� ( ������ȣ, ���̵� )
	sess->SetCurRoom(_roomMgr->GetRooms()[1]);								// ���� �� �κ�� ����
	_roomMgr->GetRooms()[1]->EnterRoom(sess);								// �κ�� ���� (�κ� �ε��� = 1)

	sess->SetIsLogin(true);

	return TRUE;
}

/// ���� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotHelp(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

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

	return TRUE;
}

/// ������ ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotExit(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

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
		sess->SetCurRoom(_roomMgr->GetRooms()[1]);				// ���� �� �κ�� ����
		_roomMgr->GetRooms()[1]->EnterRoom(sess);				// �κ�� ���� (�κ� �ε��� = 1)
	}

	return TRUE;
}

/// ��� �� ��� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotShowRoom(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	_roomMgr->ShowRoomList(sess);

	return TRUE;
}

///��� ���� ���� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotShowUser(Session * sess)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	_sessMgr->ShowUserList(sess);

	return TRUE;
}

///�� ���� ���� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotShowRoomInfo(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	return _roomMgr->ShowRoomInfo(sess, (UINT)atoi(data));
}

///���� ���� ���� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotShowUserInfo(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	return _sessMgr->ShowUserInfo(sess, string(data));
}

///�ӼӸ� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotWhisper(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	string recvClientName(data);
	string sendData(data);
	int range = sendData.find(" ");
	// (1) ���� ���ڰ� �Ǿպ��� �ڿ� ��ġ�Ѵٸ�...
	if (range > 0)
	{
		// (2) ���̵� + ���� 
		recvClientName = recvClientName.substr(0, range);
		sendData = sendData.substr(range + 1, sendData.size());
	}
	else
		return FALSE;

	if (recvClientName == sess->GetPlayerInfo().name) return FALSE; // ������ �Ӹ� �����°� �ȵ�.

	// (3) Ŭ���̾�Ʈ ã�Ƽ� �Ӹ� ������
	bool isFindSuccess = false;
	for (auto client : _sessMgr->GetClients())
	{
		if (client->GetPlayerInfo().name == recvClientName)
		{
			if (client->GetIsLogin() == false) return FALSE;
			string finalSendData = "\r\n [�ӼӸ�] [" + recvClientName + "] " + sendData + "\r\n\r\n�Է�> ";
			client->GetTcpSock().Send(finalSendData.c_str());
			isFindSuccess = true;
			break;
		}
	}
	if (isFindSuccess == false) return FALSE;		// ��ã���� ����.

	string message = "=========================================================\r\n		�ӼӸ��� ���½��ϴ�.\r\n=========================================================\r\n";
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// �游��� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotMakeRoom(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	string name(data);
	Room* room = _roomMgr->AddRoom(sess, ROOM_USER_MAX, name); // �游���
	if (room == nullptr) return FALSE;

	string message = "=========================================================\r\n		��ȭ���� �����Ǿ����ϴ�.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->GetCurRoom()->LeaveRoom(sess);					// ���� �� ������
	sess->SetCurRoom(room);									// ���ο� ������ ����
	room->EnterRoom(sess);									// ���ο� �� ����

	return TRUE;
}

///�� ���� ��ɾ�(��Ŷ) �޾��� �� �����ϴ� �Լ�
BOOL PacketProceessor::GotJoinRoom(Session * sess, const char * data)
{
	// �α����� �ȵǾ������� ��� �Ұ���
	if (sess->GetIsLogin() == false) return FALSE;

	// �� �˻�
	auto roomIter = _roomMgr->GetRooms().find(atoi(data));
	if (roomIter == _roomMgr->GetRooms().end()) return FALSE;	// �� ������ ��� 

	// ���� ���� �ִ� ���̸� ���
	auto room = (*roomIter).second;
	if (room == sess->GetCurRoom()) return FALSE;

	string message = "=========================================================\r\n		��ȭ�濡 �����߽��ϴ�.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->GetCurRoom()->LeaveRoom(sess);					// ���� �� ������
	sess->SetCurRoom(room);									// ���ο� ������ ����
	(*roomIter).second->EnterRoom(sess);					// ���ο� �� ����

	return TRUE;
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

void PacketProceessor::SendWarningMessage(Session * sess)
{
	// ��ɾ� ���� �޼��� ������
	string message = "\r\n		 * ��ɾ� ���࿡ �����߽��ϴ�.\r\n\n�Է�> \0";
	sess->GetTcpSock().Send(message.c_str());
}
