#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <set>
#include "TcpSocket.h"
#include "config.h"

class Session;

class SessionManager {
	static SessionManager* instance;		
public:
	static SessionManager* GetInstance();

public:
	Session*		AddSession(SOCKET sock); 						/// 세션(클라이언트) 추가
	void			RemoveSession(Session* client);					/// 세션(클라이언트) 제거
	void			ShowUserInfo(Session* sess, UINT ID);		/// 선택한 유저 정보 출력
	void			ShowUserList(Session* sess);					/// 전체 유저 리스트 출력

public:
	set<Session*>&		GetClients() { return _clients; };
	UINT				GetNewSessID() { return _sessID++; }

private:
	UINT			_sessID = 0;		/// 세션 고유 번호
	set<Session*>	_clients;		/// 세션(클라이언트) set
};