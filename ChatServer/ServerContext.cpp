#include "ServerContext.h"
#include "Session.h"
#include "SessionManager.h"
#include "RoomManager.h"
#include "PacketProcessor.h"
#include <set>

ServerContext::ServerContext()
	:_listenSock(SocketType::TCP, IPType::IPv4) 
{
	_sessMgr = SessionManager::GetInstance();
	_roomMgr = RoomManager::GetInstance();
}

ServerContext::~ServerContext()
{
	Close();
}

BOOL ServerContext::Init(int port)
{
	_listenSock.Bind(EndPoint("", port, IPType::IPv4));
	_listenSock.Listen();

	return TRUE;
}


BOOL ServerContext::Accept(TcpSocket& client_sock, FD_SET& rset)
{
	if (FD_ISSET(_listenSock.GetSocket(), &rset))
	{
		if (_listenSock.Accept(client_sock) == INVALID_SOCKET)
		{
			ErrorUtil::err_display("accept()");
			return FALSE;
		}
		else
		{
			printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				client_sock.GetIPAddress().c_str(), client_sock.GetPort());

			// ������ �������� �߰�
			_sessMgr->AddSession(client_sock.GetSocket());

			string message = "		* ��ɾ�� �α����� ���ּ���. (LOGIN ���̵�)\r\n\n�Է�> \0";
			client_sock.Send(message.c_str(), strlen(message.c_str()));
		}
	}
	return TRUE;
}

BOOL ServerContext::Run()
{
	set<Session*>& clients = _sessMgr->GetClients();
	_roomMgr->AddRoom(nullptr, ROOM_USER_MAX, "Lobby"); // �κ� ����

	// ������ ��ſ� ����� ����
	FD_SET rset, wset;
	TcpSocket client_socket;

	while (1)
	{
		// ���� �� �ʱ�ȭ
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(_listenSock.GetSocket(), &rset);

		for (auto it : clients)
		{
			// ���͸� ������� ���� �¿� �߰�
			if (it->GetTcpSock().GetBuf() == '\n')
				FD_SET(it->GetSock(), &wset);
			else
				FD_SET(it->GetSock(), &rset);
		}

		// select()
		if (select(0, &rset, &wset, NULL, NULL) == SOCKET_ERROR) 
			ErrorUtil::err_quit("select()");

		// (1) ����(Ŭ���̾�Ʈ) ���� ����
		Accept(client_socket, rset);

		// (2) ��� ���ǵ鿡 ���ؼ� FD_SET�� �ش� ������ ����ִٸ� �ޱ�/������ ����
		for (auto it : clients)
		{
			Session* se = it;
			if (se == nullptr) continue;
			if (se->Run(rset, wset) == FALSE) break;
		}
	}
	return TRUE;
}

void ServerContext::Close()
{
	set<Session*>& clients = _sessMgr->GetClients();
	for (auto client : clients)
	{
		delete client;
	}

	clients.clear();
	WSACleanup();
}
