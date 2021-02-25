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

	// 이부분은 엑셀 파싱 처리 예정
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
	_packetHandleMap[PacketKind::Help] = [this](Session* sess, const char* data) { GotHelp(sess); };
	_packetHandleMap[PacketKind::Exit] = [this](Session* sess, const char* data) { GotExit(sess); };
	_packetHandleMap[PacketKind::ShowRoom] = [this](Session* sess, const char* data) { GotShowRoom(sess); };
	_packetHandleMap[PacketKind::ShowUser] = [this](Session* sess, const char* data) { GotShowUser(sess); };
	_packetHandleMap[PacketKind::ShowRoomInfo] = [this](Session* sess, const char* data) { GotShowRoomInfo(sess, data); };
	_packetHandleMap[PacketKind::ShowUserInfo] = [this](Session* sess, const char* data) { GotShowUserInfo(sess, data); };
	_packetHandleMap[PacketKind::Whisper] = [this](Session* sess, const char* data) { GotWhisper(sess, data); };
	_packetHandleMap[PacketKind::JoinRoom] = [this](Session* sess, const char* data) { GotJoinRoom(sess, data); };
	_packetHandleMap[PacketKind::MakeRoom] = [this](Session* sess, const char* data) { GotMakeRoom(sess, data); };
}

BOOL PacketProceessor::PacketProcess(Session* sess, const char* data)
{
	if (sess == nullptr) return FALSE;

	PacketKind pkKind;
	string str(data);

	// 클라로 부터 받은 data가 명령어에 걸리는지 체크
	for (int i = 0; i < (int)PacketKind::End; i++)
	{
		// (1) 받은 문자 맨앞에 커맨드가 있을 경우
		if (str.find(_command[ i ] ) == 0)
		{
			// (2) 커맨드 + 데이터 분리
			auto range = _command[ i ].size();
			str.erase(0, range);
			str.erase(str.size() - 2, str.size());
			pkKind = (PacketKind)i;

			// (3) 커맨드(패킷)에 해당하는 함수 실행
			_packetHandleMap[pkKind](sess, str.c_str());
			return TRUE;
		}
	}
	
	// 명령어에 걸리지 않는다면 그냥 일반 채팅으로 뿌리기
	Chat(sess);

	return TRUE;
}

/// 로그인 명령어(패킷) 받았을 때 실행하는 함수
void PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// 로그인이 되어있으면 사용 불가능
	if (sess->GetIsLogin() == true) return;

	string message = "=========================================================\r\n		로그인되었습니다.\r\n\r\n(도움말 : /help   나가기 : /exit)\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewSessID(), data } );		// 로그인한 세션 정보 셋팅 ( 고유번호, 아이디 )
	sess->SetCurRoom(_roomMgr->GetRooms()[0]);								// 현재 방 로비로 셋팅
	_roomMgr->GetRooms()[0]->EnterRoom(sess);								// 로비방 입장 (로비 인덱스 = 0)

	sess->SetIsLogin(true);
}

void PacketProceessor::GotHelp(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	string message = "=========================================================\r\n";
	message.append("		*	도움말		*		\r\n");
	message.append("=========================================================\r\n");
	message.append("/help			: 도움말\r\n");
	message.append("/all user		: 모든 유저 보기\r\n");
	message.append("/all room		: 모든 방 보기\r\n");
	message.append("/r [방번호]		: 방 정보 보기\r\n");
	message.append("/i [아이디]		: 유저 정보 보기\r\n");
	message.append("/w [아이디] [메시지]	: 귓속말\r\n");
	message.append("/make [방제목]		: 방 생성\r\n");
	message.append("/join [방번호]		: 방 참가\r\n");
	message.append("/exit			: 나가기\r\n");
	message.append("=========================================================\r\n");
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());
}

void PacketProceessor::GotExit(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	// 현재 방이 로비일 경우
	if (sess->GetCurRoom()->GetRoomInfo()._isLobby == true)
	{
		string message = "=========================================================\r\n		접속을 종료했습니다.\r\n=========================================================\r\n";
		sess->GetTcpSock().Send(message.c_str());
		_sessMgr->RemoveSession(sess);							// 접속 종료
	}
	else // 일반 방 나가기 (로비로 복귀)
	{
		string message = "=========================================================\r\n		대화방을 나갔습니다.\r\n=========================================================\r\n";
		sess->GetTcpSock().Send(message.c_str());

		sess->GetCurRoom()->LeaveRoom(sess);					// 이전 방 나가기
		sess->SetCurRoom(_roomMgr->GetRooms()[0]);				// 현재 방 로비로 셋팅
		_roomMgr->GetRooms()[0]->EnterRoom(sess);				// 로비방 입장 (로비 인덱스 = 0)
	}
}

void PacketProceessor::GotShowRoom(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	_roomMgr->ShowRoomList(sess);
}

void PacketProceessor::GotShowUser(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	_sessMgr->ShowUserList(sess);
}

void PacketProceessor::GotShowRoomInfo(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	_roomMgr->ShowRoomInfo(sess, (UINT)atoi(data));
}

void PacketProceessor::GotShowUserInfo(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	_sessMgr->ShowUserInfo(sess, (UINT)atoi(data));
}

void PacketProceessor::GotWhisper(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;
}

void PacketProceessor::GotMakeRoom(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	string name(data);
	Room* room = _roomMgr->AddRoom(sess, ROOM_USER_MAX, name); // 방만들기
	if (room == nullptr) return;

	string message = "=========================================================\r\n		대화방이 생성되었습니다.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->GetCurRoom()->LeaveRoom(sess);					// 이전 방 나가기
	sess->SetCurRoom(room);									// 새로운 방으로 설정
	room->EnterRoom(sess);									// 새로운 방 입장
}

void PacketProceessor::GotJoinRoom(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return;

	// 방 검색
	auto roomIter = _roomMgr->GetRooms().find(atoi(data));
	if (roomIter == _roomMgr->GetRooms().end()) return;		// 방 없으면 취소 

	// 현재 내가 있는 방이면 취소
	auto room = (*roomIter).second;
	if (room == sess->GetCurRoom()) return;					

	string message = "=========================================================\r\n		대화방에 참가했습니다.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	sess->GetCurRoom()->LeaveRoom(sess);					// 이전 방 나가기
	sess->SetCurRoom(room);									// 새로운 방으로 설정
	(*roomIter).second->EnterRoom(sess);					// 새로운 방 입장
}


/// 명렁어가 아닌 일반 채팅 함수(방에 있는 사람에게 전체에게 메세지를 뿌린다)
void PacketProceessor::Chat(Session * sess)
{
	// 방이 없으면 리턴
	if (sess->GetCurRoom() == nullptr) return;

	int addrlen;
	SOCKADDR_IN clientaddr;
	string toClientBuf;

	// 보낼 문자열 꾸미기
	toClientBuf.append("\r\n");
	string str = "[" + sess->GetPlayerInfo().name + "] : " + sess->GetTcpSock().GetTotalBuf();
	toClientBuf.append( str );

	// 서버에 먼저 출력
	addrlen = sizeof(clientaddr);
	getpeername(sess->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[%s : %d]", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	printf(" %s", str.c_str());

	// 방에 있는 클라들에게 보내기
	toClientBuf.append("입력> ");
	sess->GetCurRoom()->SendData(sess, toClientBuf.c_str());

	// 채팅을 보낸 클라이언트 > 커서 다시 표시
	sess->GetTcpSock().Send("입력> ");

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();
}
