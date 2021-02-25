#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <unordered_map>
#include "TcpSocket.h"
#include "config.h"

class Room;

/*
	��ȭ���� ������ ������ �ְ� ����/���� �����ϴ� �Ŵ���
*/

class RoomManager {
	static RoomManager* instance;
public:
	static RoomManager* GetInstance();

public:
	// ���� O -> ������ ���� �� ����
	// ���� X -> �κ� ����
	Room*	AddRoom(Session* master, int max, string name);			// �� ����
	void	RemoveRoom(Room* room);									// �� ����
	BOOL	ShowRoomInfo(Session* sess, UINT roomID);				// ������ �� ���� ���
	void	ShowRoomList(Session* sess);							// ��ü �� ����Ʈ ���

public:
	unordered_map<UINT, Room*>& GetRooms() { return _rooms; }
	UINT GetNewRoomID() { return ++_roomID; }

private:
	UINT _roomID = 0;									// �� ������ȣ
	unordered_map<UINT, Room*> _rooms;					// ��� �� (key:id, value:Room ������)
};