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

	printf("%s", " [ Telnet 채팅 서버 ]\n");
	printf("%s", " * Telnet Client 접속을 기다리고 있습니다. (Port:9000)\n");
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

			string message = "		* 명령어를 사용해서 로그인하세요. ( /login 아이디 ) \r\n\n입력> ";
			client_sock.Send(message.c_str());
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
			// 엔터 or 백스페이스, 방향키 시 쓰기 셋에 추가
			//if ( KeyCheck( it->GetTcpSock().GetBuf() ) == true )
			//if(it->GetTcpSock().GetBuf() == '\n' || it->GetTcpSock().GetBuf() == '\b')
			if(KeyCheck(it->GetTcpSock().GetBuf()))
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

BOOL ServerContext::KeyCheck(char buf)
{
	if (buf == 27 ||
		buf == 17 || 
		buf == 91 || 
		buf == 18 || 
		buf == 20 || 
		buf == 8 ||
		buf == 13 || // 엔터
		buf == 9 ||  // 백스페이스
		buf == 21 || 
		buf == 92 ||
		buf == 93 ||
		buf == 25 ||
		buf == 45 ||
		buf == 46 ||
		(buf >= 33 && buf <= 36) ||
		(buf >= 37 && buf <= 40) ||
		(buf >= 112 && buf <= 123) || 
		(buf >= 37 && buf <= 40))
	{
		return TRUE;
	}

	return FALSE;
}
