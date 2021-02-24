#include "ServerContext.h"
#include "Session.h"
#include "SessionManager.h"
#include <set>

ServerContext::ServerContext()
	:_listenSock(SocketType::TCP, IPType::IPv4)//, _sessMgr(sessMgr)
{
	_sessMgr = SessionManager::GetInstance();
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
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				client_sock.GetIPAddress().c_str(), client_sock.GetPort());

			// 소켓을 세션으로 추가
			_sessMgr->AddSession(client_sock.GetSocket());

			char ss[50] = "* 명령어로 로그인을 해주세요. (LOGIN 아이디)\r\n\n>\0";
			client_sock.Send(ss, strlen(ss));
		}
	}
	return TRUE;
}

BOOL ServerContext::Run()
{
	set<Session*>& client = _sessMgr->GetClients();
	_roomMgr->AddRoom(nullptr, ROOM_USER_MAX, "Lobby"); // 로비 생성

	// 데이터 통신에 사용할 변수
	FD_SET rset, wset;
	TcpSocket client_socket;

	while (1)
	{
		// 소켓 셋 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(_listenSock.GetSocket(), &rset);

		for (auto it : client)
		{
			// 엔터를 쳤을경우 쓰기 셋에 추가
			if (it->GetTcpSock().GetBuf() == '\n')
				FD_SET(it->GetSock(), &wset);
			else
				FD_SET(it->GetSock(), &rset);
		}

		// select()
		if (select(0, &rset, &wset, NULL, NULL) == SOCKET_ERROR) 
			ErrorUtil::err_quit("select()");

		// 소켓 셋 검사(1): 클라이언트 접속 수용
		Accept(client_socket, rset);

		// 소켓 셋 검사(2): 데이터 통신
		for (auto iter : _roomMgr->GetRooms()) // Room 순회
		{
			Room* room = iter.second;
			if (room == nullptr) continue;

			for (auto it : room->GetMembers()) // Room 유저 순회
			{
				Session* se = it;
				if (se == nullptr) continue;
				if (se->Run(rset, wset) == FALSE) break;
			}
		}
	}
	return TRUE;
}

void ServerContext::Close()
{
	set<Session*>& client = _sessMgr->GetClients();
	for (auto c : client)
	{
		delete c;
	}

	client.clear();
	WSACleanup();
}
