#include "Room.h"
#include "RoomManager.h"

/// �κ� ����
Room::Room(UINT roomCode)
{
	_master = nullptr;

	_info._isLobby = true;
	_info._roomID = roomCode;
	_info._memberNumMax = ROOM_USER_MAX;
	_info._roomName = "Lobby";
}
 
/// �κ� �ƴ� �Ϲ� �� ����
Room::Room(Session * master, UINT roomCode, int max, string name)
{
	_master = master;

	_info._isLobby = false;
	_info._roomID = roomCode;
	_info._memberNumMax = max;
	_info._roomName = name;
}

void Room::EnterRoom(Session * sess)
{
	if (sess == nullptr) return;
	
	_members.insert(sess);

	/*if (sess->GetIsUnreal() == false)
	{*/
		string message = "\r\n			< " + _info._roomName + " > \r\n";
		sess->GetTcpSock().Send(message.c_str());

		message = "\r\n		 * " + sess->GetPlayerInfo().name + "���� �����ϼ̽��ϴ�.\r\n\n�Է�> \0";
		SendAllToRoomMembers(message.c_str());
	//}
}

/// ������ ������ member���� �����ϰ�, �濡 �ִ� ����鿡�� �˸���
void Room::LeaveRoom(Session * sess)
{
	if (sess == nullptr) return;
	if (_members.find(sess) == _members.end()) return;
	
	_members.erase(sess);

	if (sess->GetIsUnreal() == false)
	{
		string message = "\r\n		 * " + sess->GetPlayerInfo().name + "���� �������ϴ�.\r\n\n�Է�> \0";
		SendAllToRoomMembers(message.c_str());


		// ���� ����� �����̶�� �濡 �����ִ� ������� ���� ����
		if (_master == sess)
		{
			auto iter = _members.begin();
			if (iter != _members.end())
			{
				SetMaster((*iter));
				message = "\r\n		 * " + (*iter)->GetPlayerInfo().name + "������ ������ ���ӵǾ����ϴ�. \r\n\n�Է�> \0";
				SendAllToRoomMembers(message.c_str());
			}
		}
	}

	// �濡 �ƹ��� ������ ���� �ı��Ѵ�. (��, �κ� �ƴϾ�� ��)
	if (_members.size() == 0 && _info._isLobby == false) 
		RoomManager::GetInstance()->RemoveRoom(this);
}

/// �濡 �ִ� ����鿡�� ä�ø޼��� ������
void Room::SendData(Session * sess, const char * data)
{
	for (auto client : _members)
	{
		// ä���� ���� Ŭ���̾�Ʈ�� ����
		if (client->GetSock() == sess->GetSock()) continue;
		client->GetTcpSock().Send(data);

		if (client->GetIsUnreal() == true) return;

		// ���� ä��ġ�� �ִ� Ŭ�󿴴ٸ� �ƴ��� ȭ�鿡 �ٽ� �Ѹ���
		if (client->GetTcpSock().GetTotalBuf().size() != 0)
		{
			string chatData = client->GetTcpSock().GetTotalBuf();
			client->GetTcpSock().Send( chatData.c_str() );
		}
	}
}

void Room::SendAllToRoomMembers(const char * data)
{
	for (auto client : _members)
	{
		client->GetTcpSock().Send(data);

		if (client->GetIsUnreal() == true) return;

		// ���� ä��ġ�� �ִ� Ŭ�󿴴ٸ� �ƴ��� ȭ�鿡 �ٽ� �Ѹ���
		if (client->GetTcpSock().GetTotalBuf().size() != 0)
		{
			string chatData = client->GetTcpSock().GetTotalBuf();
			client->GetTcpSock().Send(chatData.c_str());
		}
	}
}


