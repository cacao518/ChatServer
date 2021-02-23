#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <unordered_map>
#include "TcpSocket.h"
#include "config.h"

class Room;
class RoomManager {
	static RoomManager* instance;
public:
	static RoomManager* GetInstance();

public:
	BOOL AddRoom(SOCKET sock);
	void RemoveRoom(Room* room);


public:
	unordered_map<Room*, int>& GetRooms() { return rooms_; };

private:
	unordered_map<Room*, int> rooms_;
};