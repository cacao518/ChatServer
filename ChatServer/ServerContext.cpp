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
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				client_sock.GetIPAddress().c_str(), client_sock.GetPort());
			// 소켓 정보 추가
			sessMgr_->AddSession(client_sock.GetSocket());

			char ss[50] = "채팅 로비에 입장하셨습니다.\r\n\n>\0";
			client_sock.Send(ss, strlen(ss));
		}
	}
	return TRUE;
}

BOOL ServerContext::Run()
{
	set<Session*>& client = sessMgr_->GetClients();

	// 데이터 통신에 사용할 변수
	FD_SET rset, wset;
	TcpSocket client_socket;
	int retval;

	while (1)
	{
		// 소켓 셋 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listenSock_.GetSocket(), &rset);

		for (auto it : client)
		{
			// 엔터를 쳤을경우 쓰기 셋에 추가
			if (it->GetTcpSock().GetBuf() == '\n')
				FD_SET(it->GetSock(), &wset);
			else
				FD_SET(it->GetSock(), &rset);
		}

		// select()
		retval = select(0, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR) ErrorUtil::err_quit("select()");

		// 소켓 셋 검사(1): 클라이언트 접속 수용
		Accept(client_socket, rset);

		// 소켓 셋 검사(2): 데이터 통신
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
