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

	string message = "\r\n		 * " + sess->GetPlayerInfo().name + "님이 " + "입장하셨습니다.\r\n\n입력> \0";
	SendAllToRoomMembers(message.c_str());
}

// 나가는 세션을 member에서 제거하고, 방에 있는 사람들에게 알리기
void Room::LeaveRoom(Session * sess)
{
	if (sess == nullptr) return;
	if (_members.find(sess) == _members.end()) return;
	
	_members.erase(sess);

	string message = "\r\n		 * " + sess->GetPlayerInfo().name + "님이 " + "나갔습니다.\r\n\n입력> \0";
	SendAllToRoomMembers(message.c_str());
}

// 방에 있는 사람들에게 채팅메세지 보내기
void Room::SendData(Session * sess, const char * data)
{
	for (auto client : _members)
	{
		// 채팅을 보낸 클라이언트는 제외
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


