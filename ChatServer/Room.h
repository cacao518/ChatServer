#pragma once
#include <set>
#include "Session.h"
#include "config.h"

class Room {

public:
	Room(UINT roomCode);
	Room(Session * master, UINT roomCode, int max, string name);

	void EnterRoom(Session* sess);							// ������ �濡 ������Ű�� �Լ�
	void LeaveRoom(Session* sess);							// ������ �濡�� �������� �Լ�
	void SendData(Session* sess, const char* data);			// ������ �濡 send�ϴ� �Լ�
	void SendAllToRoomMembers(const char* data);			// �濡 �ִ� ��� ���ǵ鿡�� send�ϴ� �Լ�

	RoomInfo		GetRoomInfo() const { return _info; }
	set<Session*>&	GetMembers() { return _members; };
	Session*		GetMaster() const { return _master; }

private:
	RoomInfo		_info;			// ������(������ȣ,�̸�,�ִ��ο�,�κ񿩺�)
	Session*		_master;		// ����
	set<Session*>	_members;		// �濡 ������ ����(Ŭ���̾�Ʈ)��
};