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
	Session*		AddSession(SOCKET sock); 						/// ����(Ŭ���̾�Ʈ) �߰�
	void			RemoveSession(Session* client);					/// ����(Ŭ���̾�Ʈ) ����
	void			ShowUserInfo(Session* sess, UINT ID);		/// ������ ���� ���� ���
	void			ShowUserList(Session* sess);					/// ��ü ���� ����Ʈ ���

public:
	set<Session*>&		GetClients() { return _clients; };
	UINT				GetNewSessID() { return _sessID++; }

private:
	UINT			_sessID = 0;		/// ���� ���� ��ȣ
	set<Session*>	_clients;		/// ����(Ŭ���̾�Ʈ) set
};