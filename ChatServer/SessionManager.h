#pragma once
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <set>
#include "TcpSocket.h"
#include "config.h"

class Session;

/*	
	클라이언트(세션)의 집합을 가지고 있고 생성/제거 관리하는 매니저     
*/

class SessionManager {
		
	typedef set<Session*> SessionSet;

public:
	static SessionManager* GetInstance();

public:
	Session*		AddSession(SOCKET sock); 						// 세션(클라이언트) 추가
	void			RemoveSession(Session* client);					// 세션(클라이언트) 제거
	BOOL			ShowUserInfo(Session* sess, string name);			// 선택한 유저 정보 출력
	void			ShowUserList(Session* sess);					// 전체 유저 리스트 출력

public:
	set<Session*>&		GetClients() { return _clients; };
	UINT				GetNewSessID() { return ++_sessID; }

private:
	static SessionManager* instance;
	UINT			_sessID = 0;		// 세션 고유 번호
	SessionSet	_clients;		   // 세션(클라이언트) set
};