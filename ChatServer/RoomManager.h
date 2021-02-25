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
	대화방의 집합을 가지고 있고 생성/제거 관리하는 매니저
*/

class RoomManager {
	static RoomManager* instance;
public:
	static RoomManager* GetInstance();

public:
	// 인자 O -> 유저가 만든 방 생성
	// 인자 X -> 로비 생성
	Room*	AddRoom(Session* master, int max, string name);			// 방 생성
	void	RemoveRoom(Room* room);									// 방 제거
	BOOL	ShowRoomInfo(Session* sess, UINT roomID);				// 선택한 방 정보 출력
	void	ShowRoomList(Session* sess);							// 전체 방 리스트 출력

public:
	unordered_map<UINT, Room*>& GetRooms() { return _rooms; }
	UINT GetNewRoomID() { return ++_roomID; }

private:
	UINT _roomID = 0;									// 방 고유번호
	unordered_map<UINT, Room*> _rooms;					// 모든 방 (key:id, value:Room 포인터)
};