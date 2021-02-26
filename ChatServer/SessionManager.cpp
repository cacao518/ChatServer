#include "SessionManager.h"
#include "TcpSocket.h"
#include "Session.h"

SessionManager* SessionManager::instance = nullptr;

SessionManager* SessionManager::GetInstance()
{
	if (instance == nullptr)
		instance = new SessionManager();

	return instance;

}

/// ����(Ŭ���̾�Ʈ) �߰�
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
	_clients.insert(ptr);

	// ��� �߰��� ���� ��ȯ
	return ptr;
}


/// ����(Ŭ���̾�Ʈ) ����
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
	delete ptr; // �޸� ����
}

/// ������ ���� ���� ����
BOOL SessionManager::ShowUserInfo(Session * sess, string name)
{
	UINT id;
	UINT curRoomID;
	string curRoomName;
	bool isFindSuccess = false;
	for (auto client : _clients)
	{
		if (client->GetPlayerInfo().name == name)
		{
			if (client->GetIsLogin() == false) return FALSE;
			id = client->GetPlayerInfo().id;
			curRoomID = client->GetCurRoom()->GetRoomInfo()._roomID;
			curRoomName = client->GetCurRoom()->GetRoomInfo()._roomName;
			isFindSuccess = true;
			break;
		}
	}
	if (isFindSuccess == false) return FALSE;
	string message = "=========================================================\r\n";
	message.append("		" + name + " ���� ���� \r\n=========================================================\r\n");
	message.append(" ID	�̸�		����� ID	���� �� �̸�\r\n");
	message.append("---------------------------------------------------------\r\n");

	message.append(" " + to_string(id) + "	" + name + "			" + to_string(curRoomID) + "	" + curRoomName + "\r\n");

	message.append("\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// ��� ���� ����Ʈ ����
void SessionManager::ShowUserList(Session * sess)
{
	string message = "=========================================================\r\n		��ü ���� ��� \r\n=========================================================\r\n";
	message.append("ID	�̸�			\r\n");
	message.append("---------------------------------------------------------\r\n");

	for (auto client : _clients)
	{
		if (client->GetIsLogin() == false) continue;
		UINT id = client->GetPlayerInfo().id;
		string name = client->GetPlayerInfo().name;

		message.append(" " + to_string(id) + "	" + name + "\r\n");
	}

	message.append("\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());
}
