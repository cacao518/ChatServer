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

	// 데이터 통신에 사용할 변수
	FD_SET rset, wset;
	TcpSocket client_socket;
	SOCKADDR_IN clientaddr;
	int addrlen, retval;

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
		if (FD_ISSET(listenSock_.GetSocket(), &rset))
		{
			if (listenSock_.Accept(client_socket) == INVALID_SOCKET)
			{
				ErrorUtil::err_display("accept()");
			}
			else
			{
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
					client_socket.GetIPAddress().c_str(), client_socket.GetPort());
				// 소켓 정보 추가
				sessMgr->AddSession(client_socket.GetSocket());

				char ss[50] = "채팅 로비에 입장하셨습니다.\r\n\n>\0";
				client_socket.Send(ss, strlen(ss));
			}
		}

		// 소켓 셋 검사(2): 데이터 통신
		for (auto iter : client)
		{
			Session* se = iter;
			char retBuf[BUFSIZE];

			if (FD_ISSET(se->GetSock(), &rset))
			{
				// 데이터 받기
				if (se->GetTcpSock().Receive() != Error::None) break;
				se->GetTcpSock().GetTotalBuf().push_back(se->GetTcpSock().GetBuf());
			}
			if (FD_ISSET(se->GetSock(), &wset))
			{
				// 데이터 보내기
				int idx = 0;
				retBuf[idx++] = '\r';
				retBuf[idx++] = '\n';

				for (auto c : se->GetTcpSock().GetTotalBuf())
				{
					retBuf[idx++] = c;
				}

				retBuf[idx] = '\0';

				// retBuf 서버에 먼저 출력
				addrlen = sizeof(clientaddr);
				getpeername(se->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
				printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf);


				// 클라들에게 보내기
				retBuf[idx++] = '>';
				retBuf[idx] = '\0';
				for (auto c : client)
				{
					// 채팅을 보낸 클라이언트는 제외
					if (c->GetSock() == se->GetSock()) continue;
					if (c->GetTcpSock().Send(retBuf, strlen(retBuf)) != Error::None) break;
				}

				// 채팅을 보낸 클라이언트 > 커서 다시 표시
				char ss[50] = ">\0";
				if (se->GetTcpSock().Send(ss, strlen(ss)) != Error::None) break;

				// 버퍼 비우기
				se->GetTcpSock().SetBuf('\0');
				se->GetTcpSock().GetTotalBuf().clear();
			}

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
