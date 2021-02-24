#include "ServerContext.h"
#include "Session.h"
#include "SessionManager.h"
#include <set>

ServerContext::ServerContext()
	:listenSock_(SocketType::TCP, IPType::IPv4)//, sessMgr_(sessMgr)
{
	sessMgr_ = SessionManager::GetInstance();
}

ServerContext::~ServerContext()
{
	Close();
}

BOOL ServerContext::Init(int port)
{
	listenSock_.Bind(EndPoint("", port, IPType::IPv4));
	listenSock_.Listen();

	return TRUE;
}


BOOL ServerContext::Accept(TcpSocket& client_sock, FD_SET& rset)
{
	if (FD_ISSET(listenSock_.GetSocket(), &rset))
	{
		if (listenSock_.Accept(client_sock) == INVALID_SOCKET)
		{
			ErrorUtil::err_display("accept()");
			return FALSE;
		}
		else
		{
			printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				client_sock.GetIPAddress().c_str(), client_sock.GetPort());
			// ���� ���� �߰�
			sessMgr_->AddSession(client_sock.GetSocket());

			char ss[50] = "ä�� �κ� �����ϼ̽��ϴ�.\r\n\n>\0";
			client_sock.Send(ss, strlen(ss));
		}
	}
	return TRUE;
}

BOOL ServerContext::Run()
{
	set<Session*>& client = sessMgr_->GetClients();

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
		Accept(client_socket, rset);

		// ���� �� �˻�(2): ������ ���
		for (auto iter : client)
		{
			Session* se = iter;
			if (se == nullptr) continue;
			if (se->Run(rset, wset) == FALSE) break;
		}
	}

	return TRUE;
}

void ServerContext::Close()
{
	set<Session*>& client = sessMgr_->GetClients();
	for (auto c : client)
	{
		delete c;
	}

	client.clear();
	WSACleanup();
}
