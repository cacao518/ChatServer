#include "RoomManager.h"

RoomManager* RoomManager::instance = nullptr;

RoomManager* RoomManager::GetInstance()
{
	if (instance == NULL)
		instance = new RoomManager();

	return instance;

}

BOOL RoomManager::AddRoom(SOCKET sock)
{
	return 0;
}

void RoomManager::RemoveRoom(Room * room)
{
}
