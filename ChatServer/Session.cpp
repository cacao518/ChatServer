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
		// ������ �ޱ�
		if (GetTcpSock().Receive() == FALSE) return FALSE;
		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}
	if (FD_ISSET(GetSock(), &wset))
	{
		// ������ ������
		int idx = 0;
		// ���� üũ ����
		retBuf[idx++] = '\r';
		retBuf[idx++] = '\n';

		for (auto chr : GetTcpSock().GetTotalBuf())
		{
			retBuf[idx++] = chr;
		}

		retBuf[idx] = '\0';

		// retBuf ������ ���� ���
		addrlen = sizeof(clientaddr);
		getpeername(GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
		printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf);


		// Ŭ��鿡�� ������
		retBuf[idx++] = '>';
		retBuf[idx] = '\0';
		for (auto client : clients)
		{
			// ä���� ���� Ŭ���̾�Ʈ�� ����
			if (client->GetSock() == GetSock()) continue;
			if (client->GetTcpSock().Send(retBuf, strlen(retBuf)) == FALSE) return FALSE;
		}

		// ä���� ���� Ŭ���̾�Ʈ > Ŀ�� �ٽ� ǥ��
		char ss[50] = ">\0";
		if (GetTcpSock().Send(ss, strlen(ss)) == FALSE) return FALSE;

		// ���� ����
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
