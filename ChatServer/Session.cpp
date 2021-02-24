#include "Session.h"
#include "SessionManager.h"
#include "PacketProcessor.h"

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
	//int idx = 0;
	//char buf[BUFSIZE];

	if (FD_ISSET(GetSock(), &rset))
	{
		// ������ �ޱ�
		if (GetTcpSock().Receive() == FALSE) return FALSE;
		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}

	// ����ġ�� �Ʒ��� Ȱ��ȭ
	if (FD_ISSET(GetSock(), &wset))
	{
		PacketProceessor::GetInstance()->PacketProcess(this, GetTcpSock().GetTotalBuf().c_str());

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
