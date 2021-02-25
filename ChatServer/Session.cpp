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

///  ��� ���ǵ鿡 ���ؼ� FD_SET�� �ش� ������ ����ִٸ� �ޱ�/������ ����
BOOL Session::Run(FD_SET& rset, FD_SET& wset)
{
	if (FD_ISSET(GetSock(), &rset)) // �ޱ�
	{
		// Ŭ��� ���� ���� �ϳ��� �ޱ�
		if (GetTcpSock().Receive() == FALSE) return FALSE;
		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}

	// Ư��Ű ġ�� �Ʒ��� Ȱ��ȭ
	if (FD_ISSET(GetSock(), &wset)) // ������
	{	
		// ���Ͱ� �ƴ� Ư��Ű�� �� ���� ���� �ϱ�
		//if (_socket.GetBuf() == '\b')
		if (_socket.GetBuf() != 13)
		{
			// ���� ����
			_socket.SetBuf('\0');
			_socket.GetTotalBuf().clear();
			_socket.Send("\r\n�Է�> ");
			return FALSE;
		}

		// Ŭ��� ���� ���� ��Ŷ�� ���� �Լ� ó��
		PacketProceessor::GetInstance()->PacketProcess(this, GetTcpSock().GetTotalBuf().c_str());

		// ������ ��ɾ� ���� ����
		if (this->_isExit) return FALSE;

		// ���� ����
		GetTcpSock().SetBuf('\0');
		GetTcpSock().GetTotalBuf().clear();
	}

	return TRUE;
}

/// �÷��̾� ���� ����
void Session::SetPlayerInfo(const PlayerInfo& info)
{
	_info.id = info.id;
	_info.name = info.name;
}
