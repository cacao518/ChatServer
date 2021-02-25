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
	// ���� O -> ������ ���� �� ����
	// ���� X -> �κ� ����
	Room*	AddRoom(Session* master, int max, string name);			/// �� ����
	void	RemoveRoom(Room* room);									/// �� ����

public:
	unordered_map<int, Room*>& GetRooms() { return _rooms; }
	int GetNewRoomID() { return _roomID++; }

private:
	int _roomID = 0;												/// �� ������ȣ
	unordered_map<int, Room*> _rooms;								/// ��� ��
};