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
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				client_sock.GetIPAddress().c_str(), client_sock.GetPort());

			// 소켓을 세션으로 추가
			_sessMgr->AddSession(client_sock.GetSocket());

			string message = "		* 명령어로 로그인을 해주세요. (LOGIN 아이디)\r\n\n입력> \0";
			client_sock.Send(message.c_str(), strlen(message.c_str()));
		}
	}
	return TRUE;
}

BOOL ServerContext::Run()
{
	set<Session*>& clients = _sessMgr->GetClients();
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

		for (auto it : clients)
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

		// (1) 세션(클라이언트) 접속 수용
		Accept(client_socket, rset);

		// (2) 모든 세션들에 대해서 FD_SET에 해당 소켓이 들어있다면 받기/보내기 실행
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
