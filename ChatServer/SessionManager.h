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
	Ŭ���̾�Ʈ(����)�� ������ ������ �ְ� ����/���� �����ϴ� �Ŵ���     
*/

class SessionManager {
		
	typedef set<Session*> SessionSet;

public:
	static SessionManager* GetInstance();

public:
	Session*		AddSession(SOCKET sock); 						// ����(Ŭ���̾�Ʈ) �߰�
	void			RemoveSession(Session* client);					// ����(Ŭ���̾�Ʈ) ����
	BOOL			ShowUserInfo(Session* sess, string name);			// ������ ���� ���� ���
	void			ShowUserList(Session* sess);					// ��ü ���� ����Ʈ ���

public:
	set<Session*>&		GetClients() { return _clients; };
	UINT				GetNewSessID() { return ++_sessID; }

private:
	static SessionManager* instance;
	UINT			_sessID = 0;		// ���� ���� ��ȣ
	SessionSet	_clients;		   // ����(Ŭ���̾�Ʈ) set
};