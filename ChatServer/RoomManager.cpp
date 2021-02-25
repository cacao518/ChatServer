#include "RoomManager.h"
#include "Room.h"

RoomManager* RoomManager::instance = nullptr;

RoomManager* RoomManager::GetInstance()
{
	if (instance == NULL)
		instance = new RoomManager();

	return instance;

}

Room* RoomManager::AddRoom(Session* master, int max, string name)
{
	int n = GetNewRoomID();

	if (master == nullptr)
		_rooms[n] = new Room(n); // 로비
	else
		_rooms[n] = new Room(master, n, max, name); //유저가 만든 방

	return _rooms[n];
}

void RoomManager::RemoveRoom(Room * room)
{
	// (1) 방에 있는 유저 다 내보낸 다음
	for (auto client : room->GetMembers())
	{
		if (client == nullptr) continue;
		room->LeaveRoom(client);			// 방에서 나오기
		client->SetCurRoom(_rooms[0]);		// 현재 방 로비로 설정
		_rooms[0]->EnterRoom(client);		// 로비 입장

	}

	// (2) 방 목록도 삭제한다.
	if (_rooms.find(room->GetRoomInfo()._roomCode) != _rooms.end())
		_rooms.erase(room->GetRoomInfo()._roomCode);
	
}
