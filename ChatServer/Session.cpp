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

//  ��� ���ǵ鿡 ���ؼ� FD_SET�� �ش� ������ ����ִٸ� �ޱ�/������ ����
BOOL Session::Run(FD_SET& rset, FD_SET& wset)
{
	if (FD_ISSET(GetSock(), &rset)) // �ޱ�
	{
		// Ŭ��� ���� ���� �ϳ��� �ޱ�
		if (GetTcpSock().Receive() == FALSE) return FALSE;
		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}

	// ����ġ�� �Ʒ��� Ȱ��ȭ
	if (FD_ISSET(GetSock(), &wset)) // ������
	{
		// Ŭ��� ���� ���� ��Ŷ�� ���� �Լ� ó��
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
