#include "PacketProcessor.h"
#include "RoomManager.h"
#include "Room.h"
#include "Session.h"

PacketProceessor* PacketProceessor::instance = nullptr;

PacketProceessor * PacketProceessor::GetInstance()
{
	if (instance == nullptr)
		instance = new PacketProceessor();

	return instance;
}

/// 패킷에 맞는 핸들러 함수들을 등록해둔다.
PacketProceessor::PacketProceessor()
{
	_roomMgr = RoomManager::GetInstance();
	_sessMgr = SessionManager::GetInstance();

	// 이부분은 엑셀 파싱 처리 예정
	_command.push_back("/login ");
	_command.push_back("/help");
	_command.push_back("/exit");
	_command.push_back("/i am unreal");
	_command.push_back("/user");
	_command.push_back("/room");
	_command.push_back("/r ");
	_command.push_back("/i ");
	_command.push_back("/w ");
	_command.push_back("/make ");
	_command.push_back("/join ");
	_command.push_back("/kick ");
	_command.push_back("/////");

	// 리시브 핸들러 함수 등록
	_packetHandleMap[PacketKind::Login] = [this](Session* sess, const char* data) { return GotLogin(sess, data); };
	_packetHandleMap[PacketKind::Help] = [this](Session* sess, const char* data) { return GotHelp(sess); };
	_packetHandleMap[PacketKind::Exit] = [this](Session* sess, const char* data) { return GotExit(sess); };
	_packetHandleMap[PacketKind::ShowRoom] = [this](Session* sess, const char* data) { return GotShowRoom(sess); };
	_packetHandleMap[PacketKind::ShowUser] = [this](Session* sess, const char* data) { return GotShowUser(sess); };
	_packetHandleMap[PacketKind::ShowRoomInfo] = [this](Session* sess, const char* data) { return GotShowRoomInfo(sess, data); };
	_packetHandleMap[PacketKind::ShowUserInfo] = [this](Session* sess, const char* data) { return GotShowUserInfo(sess, data); };
	_packetHandleMap[PacketKind::Whisper] = [this](Session* sess, const char* data) { return GotWhisper(sess, data); };
	_packetHandleMap[PacketKind::JoinRoom] = [this](Session* sess, const char* data) { return GotJoinRoom(sess, data); };
	_packetHandleMap[PacketKind::MakeRoom] = [this](Session* sess, const char* data) { return GotMakeRoom(sess, data); };
	_packetHandleMap[PacketKind::Kick] = [this](Session* sess, const char* data) { return GotKick(sess, data); };
	_packetHandleMap[PacketKind::UnrealCheck] = [this](Session* sess, const char* data) { return GotUnrealCheck(sess, data); };
}

/// 세션이 리시브를 하고 난 다음에 이 함수가 실행된다.
/// 만약 세션으로부터 받은 문자중에 명령어 문자열이 앞부분에 존재한다면 명령어에 따른 핸들러를 실행한다.
BOOL PacketProceessor::PacketProcess(Session* sess, const char* data)
{
	if (sess == nullptr) return FALSE;

	bool isCommand;
	PacketKind pkKind;
	string str(data);

	// 클라로 부터 받은 data가 명령어에 걸리는지 체크
	for (int i = 0; i < (int)PacketKind::End; i++)
	{
		isCommand = false;
		// (1) 커맨드만 있고 커맨드 뒤에 추가 데이터가 없는 명령어 체크
		if (i >= (int)PacketKind::Help && i <= (int)PacketKind::ShowRoom)
		{
			string compareStr = _command[i];
			compareStr.append("\r\n");
			// (2) 정확히 커맨드만 썼을 때 통과시킨다.
			if (str == compareStr)
			{
				pkKind = (PacketKind)i;
				isCommand = true;
			}
		}
		else // 커맨드 + 데이터가 있는 명령어 체크
		{
			// (1) 받은 문자 맨앞에 커맨드가 있을 경우
			if (str.find(_command[i]) == 0)
			{
				// (2) 커맨드 + 데이터 분리
				auto range = _command[i].size();
				str.erase(0, range);
				str.erase(str.size() - 2, str.size());
				pkKind = (PacketKind)i;
				isCommand = true;
			}
		}
		if (isCommand == true)
		{
			// (3) 커맨드(패킷)에 해당하는 함수 실행, 분리한 데이터를 넘겨준다.
			if (_packetHandleMap[pkKind](sess, str.c_str()) == FALSE)
				SendWarningMessage(sess, WarningKind::WrongCommand); // 명령어 수행에 실패하면 오류 메세지를 송신한다.

			return TRUE;
		}
	}

	// 로그인 안되어있는데 로그인 명령어 안쓰고 넘어오면 경고메세지 띄우기
	if(sess->GetIsLogin() == false)
		SendWarningMessage(sess, WarningKind::LoginFail);
	
	// 명령어에 걸리지 않는다면 그냥 일반 채팅으로 뿌리기
	Chat(sess);

	return TRUE;
}

/// 로그인 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotLogin(Session* sess, const char* data)
{
	// 로그인이 되어있으면 사용 불가능
	if (sess->GetIsLogin() == true) return FALSE;
	
	// 아이디에 공백 불가능
	string str(data);
	if (str.find(" ") != string::npos) return FALSE;

	if (sess->GetIsUnreal())
	{
		string message = to_string((int)PacketKind::Login) + '{' + data + '}';
		sess->GetTcpSock().Send(message.c_str());

		//string message = "/login";
		//sess->GetTcpSock().Send(message.c_str());
	}
	else
	{
		string message = "=========================================================\r\n		로그인되었습니다.\r\n\r\n(도움말 : /help   나가기 : /exit)\r\n=========================================================\r\n";
		sess->GetTcpSock().Send(message.c_str());
	}


	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewSessID(), data } );		// 로그인한 세션 정보 셋팅 ( 고유번호, 아이디 )
	sess->SetCurRoom(_roomMgr->GetRooms()[1]);								// 현재 방 로비로 셋팅
	_roomMgr->GetRooms()[1]->EnterRoom(sess);								// 로비방 입장 (로비 인덱스 = 1)

	sess->SetIsLogin(true);

	return TRUE;
}

/// 도움말 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotHelp(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	string message = "=========================================================\r\n";
	message.append("		*	도움말		*		\r\n");
	message.append("=========================================================\r\n");
	message.append("/help			: 도움말\r\n");
	message.append("/user			: 모든 유저 보기\r\n");
	message.append("/room			: 모든 방 보기\r\n");
	message.append("/r [방번호]		: 방 정보 보기\r\n");
	message.append("/i [아이디]		: 유저 정보 보기\r\n");
	message.append("/w [아이디] [메시지]	: 귓속말\r\n");
	message.append("/make [방제목]		: 방 생성\r\n");
	message.append("/join [방번호]		: 방 참가\r\n");
	message.append("/kick [아이디]		: 유저 내보내기 (방장만 가능)\r\n");
	message.append("/exit			: 나가기\r\n");
	message.append("=========================================================\r\n");
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// 나가기 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotExit(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();

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
		sess->SetCurRoom(_roomMgr->GetRooms()[1]);				// 현재 방 로비로 셋팅
		_roomMgr->GetRooms()[1]->EnterRoom(sess);				// 로비방 입장 (로비 인덱스 = 1)
	}

	return TRUE;
}

/// 모든 방 출력 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotShowRoom(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	_roomMgr->ShowRoomList(sess);

	return TRUE;
}

///모든 유저 보기 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotShowUser(Session * sess)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	_sessMgr->ShowUserList(sess);

	return TRUE;
}

///방 정보 보기 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotShowRoomInfo(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	return _roomMgr->ShowRoomInfo(sess, (UINT)atoi(data));
}

///유저 정보 보기 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotShowUserInfo(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	return _sessMgr->ShowUserInfo(sess, string(data));
}

///귓속말 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotWhisper(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	string recvClientName(data);
	string sendData(data);
	int range = sendData.find(" ");
	// (1) 받은 문자가 맨앞보다 뒤에 위치한다면...
	if (range > 0)
	{
		// (2) 아이디 + 내용 잘라내기
		recvClientName = recvClientName.substr(0, range);
		sendData = sendData.substr(range + 1, sendData.size());
	}
	else
		return FALSE;

	if (recvClientName == sess->GetPlayerInfo().name) return FALSE; // 나한테 귓말 날리는건 안됨.

	// (3) 클라이언트 찾아서 귓말 날리기
	bool isFindSuccess = false;
	for (auto client : _sessMgr->GetClients())
	{
		if (client->GetPlayerInfo().name == recvClientName)
		{
			if (client->GetIsLogin() == false) return FALSE;
			string finalSendData = "\r\n [귓속말] [" + recvClientName + "] " + sendData + "\r\n\r\n입력> ";
			client->GetTcpSock().Send(finalSendData.c_str());
			isFindSuccess = true;
			break;
		}
	}
	if (isFindSuccess == false) return FALSE;		// 못찾으면 리턴.

	string message = "=========================================================\r\n		귓속말을 보냈습니다.\r\n=========================================================\r\n";
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// 방만들기 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotMakeRoom(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	string name(data);

	// 방이름에 공백 불가능
	string str(data);
	if (str.find(" ") != string::npos) return FALSE;

	Room* room = _roomMgr->AddRoom(sess, ROOM_USER_MAX, name); // 방만들기
	if (room == nullptr) return FALSE;

	string message = "=========================================================\r\n		대화방이 생성되었습니다.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();

	sess->GetCurRoom()->LeaveRoom(sess);					// 이전 방 나가기
	sess->SetCurRoom(room);									// 새로운 방으로 설정
	room->EnterRoom(sess);									// 새로운 방 입장

	return TRUE;
}

///방 참여 명령어(패킷) 받았을 때 실행하는 함수
BOOL PacketProceessor::GotJoinRoom(Session * sess, const char * data)
{
	// 로그인이 안되어있으면 사용 불가능
	if (sess->GetIsLogin() == false) return FALSE;

	// 방 검색
	auto roomIter = _roomMgr->GetRooms().find(atoi(data));
	if (roomIter == _roomMgr->GetRooms().end()) return FALSE;	// 방 없으면 취소 

	// 현재 내가 있는 방이면 취소
	auto room = (*roomIter).second;
	if (room == sess->GetCurRoom()) return FALSE;

	string message = "=========================================================\r\n		대화방에 참가했습니다.\r\n=========================================================\r\n";
	sess->GetTcpSock().Send(message.c_str());

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();

	sess->GetCurRoom()->LeaveRoom(sess);					// 이전 방 나가기
	sess->SetCurRoom(room);									// 새로운 방으로 설정
	(*roomIter).second->EnterRoom(sess);					// 새로운 방 입장

	return TRUE;
}

BOOL PacketProceessor::GotKick(Session * sess, const char * data)
{
	//방장이 아니면 명령어 사용 불가
	if (sess->GetCurRoom()->GetMaster() != sess) return FALSE;

	string name(data);
	Session* kickedUser = nullptr;

	bool isFindSuccess = false;
	for (auto client : sess->GetCurRoom()->GetMembers()) // 같은 방에 있는 사람들 순회
	{
		if (client->GetPlayerInfo().name == name) // 내보내려 하는 사람을 찾았다면
		{
			if (client->GetIsLogin() == false) return FALSE; // 로그인 되있는지 체크
			kickedUser = client;		// 강퇴할 세션 주소 획득
			isFindSuccess = true;
			break;
		}
	}
	if (name == sess->GetPlayerInfo().name) return FALSE;	 // 자기 자신 추방은 안돼.
	if (isFindSuccess == false) return FALSE;				// 강퇴할 대상 탐색 실패

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();

	string message = "\r\n		 * " + kickedUser->GetPlayerInfo().name + "님이 추방당했습니다. \r\n\n입력> \0";
	sess->GetCurRoom()->SendAllToRoomMembers(message.c_str());

	kickedUser->GetCurRoom()->LeaveRoom(kickedUser);			// 이전 방 나가게 하기
	kickedUser->SetCurRoom(_roomMgr->GetRooms()[1]);			// 현재 방 로비로 셋팅
	_roomMgr->GetRooms()[1]->EnterRoom(kickedUser);				// 로비방 입장 (로비 인덱스 = 1)

	return TRUE;
}

BOOL PacketProceessor::GotUnrealCheck(Session * sess, const char * data)
{
	sess->SetIsUnreal(true);
	return TRUE;
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

	if (sess->GetIsUnreal())
	{
		string message = to_string((int)PacketKind::SendData) + '{' + str + '}';
		sess->GetCurRoom()->SendData(sess, toClientBuf.c_str());
	}
	else
	{
		// 방에 있는 클라들에게 보내기
		toClientBuf.append("입력> ");
		sess->GetCurRoom()->SendData(sess, toClientBuf.c_str());

		// 채팅을 보낸 클라이언트 > 커서 다시 표시
		sess->GetTcpSock().Send("입력> ");
	}

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();
}

void PacketProceessor::SendWarningMessage(Session* sess, WarningKind warningKind)
{
	string message;
	// 명령어 오류 메세지 보내기
	switch (warningKind)
	{
		case WarningKind::WrongCommand:
			message = "\r\n		 * 명령어 수행에 실패했습니다.\r\n\n입력> \0";
			sess->GetTcpSock().Send(message.c_str());
			break;
		
		case WarningKind::LoginFail:
			message = "\r\n		* 명령어를 사용해서 로그인하세요. ( /login 아이디 ) \r\n\n입력> ";
			sess->GetTcpSock().Send(message.c_str());
			break;

		default:
			break;
	}

}
