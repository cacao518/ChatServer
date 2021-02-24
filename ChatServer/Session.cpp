#include "Session.h"
#include "SessionManager.h"

Session::Session(TcpSocket socket)
{
	_socket = socket;
	_socket.SetSession(this);
}

Session::~Session()
{
}

BOOL Session::Run(FD_SET& rset, FD_SET& wset)
{
	SessionManager* sessMgr = SessionManager::GetInstance();
	set<Session*>& clients = sessMgr->GetClients();

	int addrlen;
	SOCKADDR_IN clientaddr;
	char retBuf[BUFSIZE];

	if (FD_ISSET(GetSock(), &rset))
	{
		// 데이터 받기
		if (GetTcpSock().Receive() == FALSE) return FALSE;
		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}
	if (FD_ISSET(GetSock(), &wset))
	{
		// 데이터 보내기
		int idx = 0;
		// 범위 체크 ㄱㄱ
		retBuf[idx++] = '\r';
		retBuf[idx++] = '\n';

		for (auto chr : GetTcpSock().GetTotalBuf())
		{
			retBuf[idx++] = chr;
		}

		retBuf[idx] = '\0';

		// retBuf 서버에 먼저 출력
		addrlen = sizeof(clientaddr);
		getpeername(GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
		printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf);


		// 클라들에게 보내기
		retBuf[idx++] = '>';
		retBuf[idx] = '\0';
		for (auto client : clients)
		{
			// 채팅을 보낸 클라이언트는 제외
			if (client->GetSock() == GetSock()) continue;
			if (client->GetTcpSock().Send(retBuf, strlen(retBuf)) == FALSE) return FALSE;
		}

		// 채팅을 보낸 클라이언트 > 커서 다시 표시
		char ss[50] = ">\0";
		if (GetTcpSock().Send(ss, strlen(ss)) == FALSE) return FALSE;

		// 버퍼 비우기
		GetTcpSock().SetBuf('\0');
		GetTcpSock().GetTotalBuf().clear();
	}

	return TRUE;
}

void Session::SetPlayerInfo(PlayerInfo info)
{
	_info.code = info.code;
	_info.name = info.name;
}
