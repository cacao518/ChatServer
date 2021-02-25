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

///  모든 세션들에 대해서 FD_SET에 해당 소켓이 들어있다면 받기/보내기 실행
BOOL Session::Run(FD_SET& rset, FD_SET& wset)
{
	if (FD_ISSET(GetSock(), &rset)) // 받기
	{
		// 클라로 부터 글자 하나씩 받기
		if (GetTcpSock().Receive() == FALSE) return FALSE;
		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}

	// 엔터치면 아래줄 활성화
	if (FD_ISSET(GetSock(), &wset)) // 보내기
	{
		// 클라로 부터 받은 패킷에 따른 함수 처리
		PacketProceessor::GetInstance()->PacketProcess(this, GetTcpSock().GetTotalBuf().c_str());

		if (this->_isExit) return FALSE;

		// 버퍼 비우기
		GetTcpSock().SetBuf('\0');
		GetTcpSock().GetTotalBuf().clear();
	}

	return TRUE;
}

/// 플레이어 정보 셋팅
void Session::SetPlayerInfo(const PlayerInfo& info)
{
	_info.id = info.id;
	_info.name = info.name;
}
