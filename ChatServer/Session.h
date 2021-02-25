#pragma once
#include "TcpSocket.h"
#include "Room.h"

class Room;

/*	클라이언트의 역할을 하는 클래스      */

class Session {

public:	
	Session(TcpSocket socket);
	~Session();

	TcpSocket&	GetTcpSock() { return _socket; };
	SOCKET&		GetSock() { return _socket.GetSocket(); };
	BOOL		Run(FD_SET& rset, FD_SET& wset);				// FD_SET에 소켓이 들어 있다면 읽기/쓰기를 실행

	////////////////////////////////////

	bool		GetIsLogin() { return _isLogin; };
	PlayerInfo	GetPlayerInfo() { return _info; }
	Room*		GetCurRoom() { return _curRoom; }
	void		SetIsLogin(const bool& isLogin) { _isLogin = isLogin; };
	void		SetIsExit(const bool& isExit) { _isExit = isExit; };
	void		SetPlayerInfo(const PlayerInfo& info);
	void		SetCurRoom(Room* parent) { _curRoom = parent; }

	void		StartBufLimit();			// 버퍼 담기 금지 시작
	void		ReleaseBufLimit();			// 버퍼 담기 금지 해제 함수

private:
	TcpSocket	_socket;					// Tcp 소켓
	PlayerInfo	_info;						// 유저 정보(아이디, 이름)
	Room*		_curRoom;					// 현재 방 

	bool		_isLogin = false;					// 로그인 여부
	bool		_isExit = false;					// 프로그램 종료 여부

	bool		_isBufLimit = false;				// 버퍼에 담을수 없는 상태
	short		_ReleaseBufLimitCount = 0;			// 버퍼에 담을수 없는 상태 헤제 카운트 변수
};
