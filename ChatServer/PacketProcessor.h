#pragma once
#include "RoomManager.h"
#include "SessionManager.h"
// �ڳ� Ŭ���̾�Ʈ�� ����
// �׳� ä�ø� ������ �״�� ���

// ��ɾ� + ä�� ������ ��Ŷ ó�� �����ϴ� ������ �Լ� ȣ�� ����
// H -> �����Լ�
// LOGIN + ä�� -> �κ�� ���� �Լ�
// ENTER + ���ȣ -> ������ �Լ�

class PacketProceessor {


public:
	PacketProceessor();

	void GotLogin(Session* sess, const char* data);


private:
	RoomManager*	_roomMgr;
	SessionManager* _sessMgr;
};