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

	printf("%s", " [ Telnet ä�� ���� ]\n");
	printf("%s", " * Telnet Client ������ ��ٸ��� �ֽ��ϴ�. (Port:9000)\n");
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

			string message = "		* ��ɾ ����ؼ� �α����ϼ���. ( /login ���̵� ) \r\n\n�Է�> ";
			client_sock.Send(message.c_str());
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
			// �ѱ�, ����, ���� �� �Է� �� ���� Ư�� Ű�� ��� wset�� �߰��ϱ�
			if (KeyCheck(it->GetTcpSock().GetBuf()) == TRUE)
			{
				FD_SET(it->GetSock(), &wset);
				if(it->GetTcpSock().GetBuf() != '\n' && 
					it->GetTcpSock().GetBuf() != '\r' && 
					it->GetTcpSock().GetBuf() != '\b')
					it->StartBufLimit();
			}
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

BOOL ServerContext::KeyCheck(char buf)
{
	if (buf == '\0' || buf == '\r') return FALSE;	// ���۱⺻����, ����Ű �պκ�
	if (buf >= 32 && buf <= 126) return FALSE;		// ����,����,�⺻Ư������ 
	if (buf & 0x80) return FALSE;					// �ѱ�üũ 

	// ������ Ư��Ű�� 
	return TRUE;
}
