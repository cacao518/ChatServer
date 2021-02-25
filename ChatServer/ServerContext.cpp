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
			// 한글, 영어, 숫자 등 입력 값 빼고 특수 키는 모두 wset에 추가하기
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
	if (buf == '\0' || buf == '\r') return FALSE;	// 버퍼기본상태, 엔터키 앞부분
	if (buf >= 32 && buf <= 126) return FALSE;		// 영어,숫자,기본특수문자 
	if (buf & 0x80) return FALSE;					// 한글체크 

	// 나머지 특수키들 
	return TRUE;
}
