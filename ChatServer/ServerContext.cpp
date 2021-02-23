#include "ServerContext.h"
#include "Session.h"
#include "SessionManager.h"
#include <set>

ServerContext::ServerContext()
	:listenSock_(SocketType::TCP, IPType::IPv4)//, sessMgr_(sessMgr)
{
}

ServerContext::~ServerContext()
{
	Close();
}

Error ServerContext::Init(int port)
{
	listenSock_.Bind(EndPoint("", port, IPType::IPv4));
	listenSock_.Listen();

	return Error::None;
}


Error ServerContext::Accept(SOCKET & client_sock)
{
	return Error::None;
}

Error ServerContext::Run()
{
	SessionManager* sessMgr = SessionManager::GetInstance();
	set<Session*>& client = sessMgr->GetClients();

	// ������ ��ſ� ����� ����
	FD_SET rset, wset;
	TcpSocket client_socket;
	int retval;

	while (1)
	{
		// ���� �� �ʱ�ȭ
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listenSock_.GetSocket(), &rset);

		for (auto it : client)
		{
			// ���͸� ������� ���� �¿� �߰�
			if (it->GetTcpSock().GetBuf() == '\n')
				FD_SET(it->GetSock(), &wset);
			else
				FD_SET(it->GetSock(), &rset);
		}

		// select()
		retval = select(0, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR) ErrorUtil::err_quit("select()");

		// ���� �� �˻�(1): Ŭ���̾�Ʈ ���� ����
		if (FD_ISSET(listenSock_.GetSocket(), &rset))
		{
			if (listenSock_.Accept(client_socket) == INVALID_SOCKET)
			{
				ErrorUtil::err_display("accept()");
			}
			else
			{
				printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
					client_socket.GetIPAddress().c_str(), client_socket.GetPort());
				// ���� ���� �߰�
				sessMgr->AddSession(client_socket.GetSocket());

				char ss[50] = "ä�� �κ� �����ϼ̽��ϴ�.\r\n\n>\0";
				client_socket.Send(ss, strlen(ss));
			}
		}

		// ���� �� �˻�(2): ������ ���
		for (auto iter : client)
		{
			Session* se = iter;
			if (se->Run(rset, wset) != Error::None) break;
		}
	}

	return Error::None;
}

void ServerContext::Close()
{
	SessionManager* sessMgr = SessionManager::GetInstance();
	set<Session*>& client = sessMgr->GetClients();
	for (auto c : client)
	{
		delete c;
	}

	client.clear();
	WSACleanup();
}
