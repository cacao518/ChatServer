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

	// 아이디 출력추가하기
	char ss[50] = "채팅 로비에 입장하셨습니다.\r\n\n>\0";
	sess->GetTcpSock().Send(ss, strlen(ss));

	// SendAllToRoomMembers 입장했다고 ㄱ
}

void Room::LeaveRoom(Session * sess)
{
	if (sess == nullptr) return;
	if (_members.find(sess) == _members.end()) return;
	
	_members.erase(sess);

	// SendAllToRoomMembers 나갔다고 ㄱ
}

void Room::SendData(Session * sess, const char * data)
{
	// sess에서 이름과 아이디를 가져와서 data에 붙이고
	// sendAlltoroomMebers 함수 출력해서 다 뿌려
}

void Room::SendAllToRoomMembers(const char * data)
{
}


