#include "PacketProcessor.h"
#include "RoomManager.h"
#include "Room.h"
#include "Session.h"

PacketProceessor* PacketProceessor::instance = nullptr;

PacketProceessor * PacketProceessor::GetInstance()
{
	if (instance == NULL)
		instance = new PacketProceessor();

	return instance;
}

PacketProceessor::PacketProceessor()
{
	_roomMgr = RoomManager::GetInstance();
	_sessMgr = SessionManager::GetInstance();

	_command.push_back("/login ");
	_command.push_back("/help");
	_command.push_back("/exit");
	_command.push_back("/all user");
	_command.push_back("/all room");
	_command.push_back("/r ");
	_command.push_back("/i ");
	_command.push_back("/w ");
	_command.push_back("/make ");
	_command.push_back("/join ");

	_packetHandleMap[PacketKind::Login] = [this](Session* sess, const char* data) { GotLogin(sess, data); };
}

BOOL PacketProceessor::PacketProcess(Session* sess, const char* data)
{
	if (sess == nullptr) return FALSE;

	PacketKind pkKind;
	string str(data);

	// 클라로 부터 받은 data가 명령어에 걸리는지 체크
	for (int i = 0; i < (int)PacketKind::End - 1; i++)
	{
		if (str.find(_command[ i ] ) == 0) // 받은 문자 맨앞에 커맨드가 있을 경우
		{
			auto range = _command[ i ].size();
			str.erase(0, range);
			str.erase(str.size() - 2, str.size());
			pkKind = (PacketKind)i;

			_packetHandleMap[pkKind](sess, str.c_str());
			return TRUE;
		}
	}
	
	// 명령어에 걸리지 않는다면 그냥 일반 채팅으로 뿌리기
	Chat(sess);

	return TRUE;
}

// 로그인 명령어(패킷) 받았을 때 실행하는 함수
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// 로그인하면 처음에 무조건 로비로 입장

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );		// 로그인한 세션 정보 셋팅

	sess->SetParent(_roomMgr->GetRooms()[0]);		// 세션 부모(입장한 방) 셋팅

	_roomMgr->GetRooms()[0]->EnterRoom(sess);		// 로비방 입장 (로비 인덱스 = 0)
}


// 명렁어가 아닌 일반 채팅 함수(방에 있는 사람에게 전체에게 메세지를 뿌린다)
void PacketProceessor::Chat(Session * sess)
{
	// 방이 없으면 리턴
	if (sess->GetParent() == nullptr) return;

	int addrlen;
	SOCKADDR_IN clientaddr;

	string retBuf;

	// 데이터 보내기
	retBuf.append("\r\n");
	
	string str = "[" + sess->GetPlayerInfo().name + "] : " + sess->GetTcpSock().GetTotalBuf();
	retBuf.append( str );

	// retBuf 서버에 먼저 출력
	addrlen = sizeof(clientaddr);
	getpeername(sess->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf.c_str());

	// 방에 있는 클라들에게 보내기
	retBuf.append(">");
	sess->GetParent()->SendData(sess, retBuf.c_str());

	// 채팅을 보낸 클라이언트 > 커서 다시 표시
	sess->GetTcpSock().Send(">", strlen(">"));

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();
}
