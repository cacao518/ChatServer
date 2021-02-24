#pragma once
#include "RoomManager.h"
#include "SessionManager.h"
// 텔넷 클라이언트로 부터
// 그냥 채팅만 받으면 그대로 출력

// 명령어 + 채팅 받으면 패킷 처럼 구분하는 식으로 함수 호출 ㄱㄱ
// H -> 도움말함수
// LOGIN + 채팅 -> 로비방 입장 함수
// ENTER + 방번호 -> 방입장 함수

class PacketProceessor {


public:
	PacketProceessor();

	void GotLogin(Session* sess, const char* data);


private:
	RoomManager*	_roomMgr;
	SessionManager* _sessMgr;
};