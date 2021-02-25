#include "SessionManager.h"
#include "TcpSocket.h"
#include "Session.h"


SessionManager* SessionManager::instance = nullptr;

SessionManager* SessionManager::GetInstance()
{
	if (instance == NULL)
		instance = new SessionManager();

	return instance;

}

///////////////////////////////////
/// @brief ���� �Լ� ������������
///
///	@param	sock	���ϴ� ����
///	@param	sock	���ϴ� ����
//////////////////////////////////
Session* SessionManager::AddSession(SOCKET sock)
{
	if (_clients.size() >= FD_SETSIZE) {
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return nullptr;
	}

	Session* ptr = new Session( TcpSocket(SocketType::TCP, IPType::IPv4) );

	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return nullptr;
	}

	ptr->GetTcpSock().SetSocket(sock);
	ptr->GetTcpSock().SetRecvBytes(0);
	_clients.insert(ptr);

	// ��� �߰��� ���� ��ȯ
	return ptr;
}


void SessionManager::RemoveSession(Session* client)
{
	if (_clients.empty()) return;

	Session* ptr = *(_clients.find(client));
	if (ptr == nullptr) return;

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	if (ptr->GetCurRoom() != nullptr)
	{
		// Room�� �ִ� member���� �����ϰ�, �濡 �ִ� ����鿡�� �˸���
		ptr->GetCurRoom()->LeaveRoom(client);

		// �������� �޽��� �濡 �ִ� ����鿡�� ���
		string message = "\r\n		 * " + client->GetPlayerInfo().name + "���� " + "������ �����߽��ϴ�.\r\n\n�Է�> \0";
		ptr->GetCurRoom()->SendAllToRoomMembers(message.c_str());
	}

	// ���� �ݱ�
	closesocket(ptr->GetSock());
	
	// ���� set ���� ����
	_clients.erase(client);

	client->SetIsExit(true);
	delete ptr;
}

void SessionManager::ShowUserInfo(Session * sess, UINT ID)
{
	UINT id;
	string name;
	UINT curRoomID;
	string curRoomName;
	bool isFindSuccess = false;
	for (auto client : _clients)
	{
		if (client->GetPlayerInfo().id == ID)
		{
			if (client->GetIsLogin() == false) return;
			id = client->GetPlayerInfo().id;
			name = client->GetPlayerInfo().name;
			curRoomID = client->GetCurRoom()->GetRoomInfo()._roomID;
			curRoomName = client->GetCurRoom()->GetRoomInfo()._roomName;
			isFindSuccess = true;
			break;
		}
	}
	if (isFindSuccess == false) return;
	string message = "=========================================================\r\n";
	message.append("		" + name + " ���� ���� \r\n=========================================================\r\n");
	message.append(" ID	�̸�		����� ID	���� �� �̸�\r\n");
	message.append("---------------------------------------------------------\r\n");

	message.append(" " + to_string(id) + "	" + name + "			" + to_string(curRoomID) + "	" + curRoomName + "\r\n");

	message.append("\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());
}

void SessionManager::ShowUserList(Session * sess)
{
	string message = "=========================================================\r\n		��ü ���� ��� \r\n=========================================================\r\n";
	message.append("ID	�̸�			\r\n");
	message.append("---------------------------------------------------------\r\n");

	for (auto client : _clients)
	{
		UINT id = client->GetPlayerInfo().id;
		string name = client->GetPlayerInfo().name;

		message.append(" " + to_string(id) + "	" + name + "\r\n");
	}

	message.append("\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());
}
