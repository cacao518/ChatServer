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
	// 牢磊 O -> 蜡历啊 父电 规 积己
	// 牢磊 X -> 肺厚 积己
	Room*	AddRoom(Session* master, int max, string name);			/// 规 积己
	void	RemoveRoom(Room* room);									/// 规 力芭

public:
	unordered_map<int, Room*>& GetRooms() { return _rooms; }
	int GetNewRoomID() { return _roomID++; }

private:
	int _roomID = 0;												/// 规 绊蜡锅龋
	unordered_map<int, Room*> _rooms;								/// 葛电 规
};