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
		_rooms[n] = new Room(n); // �κ�
	else
		_rooms[n] = new Room(master, n, max, name); //������ ���� ��

	return _rooms[n];
}

void RoomManager::RemoveRoom(Room * room)
{
	// (1) �濡 �ִ� ���� �� ������ ����
	for (auto client : room->GetMembers())
	{
		if (client == nullptr) continue;
		room->LeaveRoom(client);			// �濡�� ������
		client->SetCurRoom(_rooms[0]);		// ���� �� �κ�� ����
		_rooms[0]->EnterRoom(client);		// �κ� ����

	}

	// (2) �� ��ϵ� �����Ѵ�.
	if (_rooms.find(room->GetRoomInfo()._roomCode) != _rooms.end())
		_rooms.erase(room->GetRoomInfo()._roomCode);
	
}
