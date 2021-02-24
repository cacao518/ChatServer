#pragma once
#include <set>
#include "Session.h"
#include "config.h"

class Room {

public:
	Room(int roomCode);
	Room(Session * master, int roomCode, int max, string name);

	void EnterRoom(Session* sess);							/// 세션을 방에 참여시키는 함수
	void LeaveRoom(Session* sess);							/// 세션을 방에서 내보내는 함수
	void SendData(Session* sess, const char* data);			/// 세션이 방에 send하는 함수
	void SendAllToRoomMembers(const char* data);			/// 방에 있는 모든 세션들에게 send하는 함수

	RoomInfo		GetRoomInfo() const { return _info; }
	set<Session*>&	GetMembers() { return _members; };

private:
	RoomInfo		_info;			/// 방정보(고유번호,이름,최대인원,로비여부)
	Session*		_master;		/// 방장
	set<Session*>	_members;		/// 방에 접속한 세션(클라이언트)들
};