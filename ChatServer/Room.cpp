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
	this->_master = master;

	_info._isLobby = false;
	_info._roomID = roomCode;
	_info._memberNumMax = max;
	_info._roomName = name;
}

void Room::EnterRoom(Session * sess)
{
	if (sess == nullptr) return;
	
	_members.insert(sess);

	string message = "\r\n			< " + _info._roomName + " > \r\n";
	sess->GetTcpSock().Send(message.c_str());

	message = "\r\n		 * " + sess->GetPlayerInfo().name + "���� " + "�����ϼ̽��ϴ�.\r\n\n�Է�> \0";
	SendAllToRoomMembers(message.c_str());
}

/// ������ ������ member���� �����ϰ�, �濡 �ִ� ����鿡�� �˸���
void Room::LeaveRoom(Session * sess)
{
	if (sess == nullptr) return;
	if (_members.find(sess) == _members.end()) return;
	
	_members.erase(sess);

	string message = "\r\n		 * " + sess->GetPlayerInfo().name + "���� " + "�������ϴ�.\r\n\n�Է�> \0";
	SendAllToRoomMembers(message.c_str());

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
	}
}

void Room::SendAllToRoomMembers(const char * data)
{
	for (auto client : _members)
	{
		client->GetTcpSock().Send(data);
	}
}


