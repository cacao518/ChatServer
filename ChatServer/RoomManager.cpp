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
	int n = GetNewCode();

	if (master == nullptr)
		_rooms[n] = new Room(n); // �κ�
	else
		_rooms[n] = new Room(master, n, max, name); //������ ���� ��

	return _rooms[n];
}

void RoomManager::RemoveRoom(Room * room)
{
	// �濡 �ִ� ���� �� ������ ����

	if (_rooms.find(room->GetRoomInfo()._roomCode) != _rooms.end())
		_rooms.erase(room->GetRoomInfo()._roomCode);
	
}
