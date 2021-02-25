#include "Room.h"
#include "RoomManager.h"

/// 로비 생성
Room::Room(UINT roomCode)
{
	_master = nullptr;

	_info._isLobby = true;
	_info._roomID = roomCode;
	_info._memberNumMax = ROOM_USER_MAX;
	_info._roomName = "Lobby";
}
 
/// 로비가 아닌 일반 방 생성
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

	message = "\r\n		 * " + sess->GetPlayerInfo().name + "님이 " + "입장하셨습니다.\r\n\n입력> \0";
	SendAllToRoomMembers(message.c_str());
}

/// 나가는 세션을 member에서 제거하고, 방에 있는 사람들에게 알리기
void Room::LeaveRoom(Session * sess)
{
	if (sess == nullptr) return;
	if (_members.find(sess) == _members.end()) return;
	
	_members.erase(sess);

	string message = "\r\n		 * " + sess->GetPlayerInfo().name + "님이 " + "나갔습니다.\r\n\n입력> \0";
	SendAllToRoomMembers(message.c_str());

	// 방에 아무도 없으면 방을 파괴한다. (단, 로비가 아니어야 함)
	if (_members.size() == 0 && _info._isLobby == false) 
		RoomManager::GetInstance()->RemoveRoom(this);
}

/// 방에 있는 사람들에게 채팅메세지 보내기
void Room::SendData(Session * sess, const char * data)
{
	for (auto client : _members)
	{
		// 채팅을 보낸 클라이언트는 제외
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


