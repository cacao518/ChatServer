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
		if (GetTcpSock().Receive() == FALSE) 
			return FALSE;
		else
		{
			// 만약 이전에 특수키를 눌렀다면 버퍼를 3회 정도 비운다.
			if (_isBufLimit == true)
			{
				ReleaseBufLimit();
				return FALSE;
			}
		}

		GetTcpSock().GetTotalBuf().push_back(GetTcpSock().GetBuf());
	}

	// 특수키 치면 아래줄 활성화
	if (FD_ISSET(GetSock(), &wset)) // 보내기
	{	
		// 엔터키(마지막에 \n가 붙음)가 아닌 특수키는 다 버퍼 비우게 하기
		if (_socket.GetBuf() != '\n')
		{
			// 버퍼 비우기
			_socket.SetBuf('\0');
			_socket.GetTotalBuf().clear();
			_socket.Send("\r\n입력> ");
			return FALSE;
		}

		// 클라로 부터 받은 패킷에 따른 함수 처리
		PacketProceessor::GetInstance()->PacketProcess(this, GetTcpSock().GetTotalBuf().c_str());

		// 나가는 명령어 시전 방지
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

/// 버퍼 담기 금지 시작
void Session::StartBufLimit()
{
	_isBufLimit = true;
	_ReleaseBufLimitCount = BUF_LIMIT_COUNT;
}

/// 버퍼 담기 금지 해제 함수
void Session::ReleaseBufLimit()
{
	if (_ReleaseBufLimitCount != 0)
		_ReleaseBufLimitCount--;
	else
		_isBufLimit = false;
}
