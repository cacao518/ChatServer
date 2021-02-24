#include "Room.h"

Room::Room(int roomCode)
{
	_master = nullptr;

	_info._isLobby = true;
	_info._roomCode = roomCode;
	_info._memberNumMax = ROOM_USER_MAX;
	_info._roomName = "Lobby";
}

Room::Room(Session * master, int roomCode, int max, string name)
{
	this->_master = master;

	_info._isLobby = false;
	_info._roomCode = roomCode;
	_info._memberNumMax = max;
	_info._roomName = name;
}

void Room::EnterRoom(Session * sess)
{
	if (sess == nullptr) return;
	
	_members.insert(sess);

	string message = "* " + sess->GetPlayerInfo().name + "���� " + " �����ϼ̽��ϴ�.\r\n\n>\0";
	SendAllToRoomMembers(message.c_str());
}

void Room::LeaveRoom(Session * sess)
{
	if (sess == nullptr) return;
	if (_members.find(sess) == _members.end()) return;
	
	_members.erase(sess);

	string message = "* " + sess->GetPlayerInfo().name + "���� " + " �������ϴ�.\r\n\n>\0";
	SendAllToRoomMembers(message.c_str());
}

void Room::SendData(Session * sess, const char * data)
{
	for (auto client : _members)
	{
		// ä���� ���� Ŭ���̾�Ʈ�� ����
		if (client->GetSock() == sess->GetSock()) continue;
		client->GetTcpSock().Send(data, strlen(data));
	}
}

void Room::SendAllToRoomMembers(const char * data)
{
	for (auto client : _members)
	{
		client->GetTcpSock().Send(data, strlen(data));
	}
}


