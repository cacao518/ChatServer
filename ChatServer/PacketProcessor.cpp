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
		if (str.find(_command[ i ] ) == 0) // 받은 문자 맨앞에 커맨드가 있을 경우
		{
			auto range = _command[ i ].size();
			str.erase(0, range);
			str.erase(str.size() - 2, str.size());
			pkKind = (PacketKind)i;

			// 패킷에 해당하는 함수 실행
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
	if (sess->GetIsLogin() == true) return;

	sess->SetPlayerInfo( PlayerInfo{ _sessMgr->GetNewCode(), data } );		// 로그인한 세션 정보 셋팅
	sess->SetCurRoom(_roomMgr->GetRooms()[0]);								// 세션 부모(입장한 방) 셋팅
	_roomMgr->GetRooms()[0]->EnterRoom(sess);								// 로비방 입장 (로비 인덱스 = 0)

	sess->SetIsLogin(true);
}

void PacketProceessor::GotHelp(Session * sess)
{
}

void PacketProceessor::GotExit(Session * sess)
{
}

void PacketProceessor::GotShowRoom(Session * sess)
{
}

void PacketProceessor::GotShowUser(Session * sess)
{
}

void PacketProceessor::GotShowRoomInfo(Session * sess, const char * data)
{
}

void PacketProceessor::GotShowUserInfo(Session * sess, const char * data)
{
}

void PacketProceessor::GotWhisper(Session * sess, const char * data)
{
}

void PacketProceessor::GotMakeRoom(Session * sess, const char * data)
{
	string name(data);
	Room* room = _roomMgr->AddRoom(sess, ROOM_USER_MAX, name); // 방만들기
	if (room == nullptr) return;
	sess->GetCurRoom()->LeaveRoom(sess);					// 이전 방 나가기
	sess->SetCurRoom(room);									// 새로운 방으로 설정
	room->EnterRoom(sess);									// 새로운 방 입장
}

void PacketProceessor::GotJoinRoom(Session * sess, const char * data)
{
	// 방 검색
	auto roomIter = _roomMgr->GetRooms().find(atoi(data));
	if (roomIter == _roomMgr->GetRooms().end()) return;		// 방 없으면 취소 

	// 현재 내가 있는 방이면 취소
	auto room = (*roomIter).second;
	if (room == sess->GetCurRoom()) return;					

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
	string retBuf;

	// 보낼 문자열 꾸미기
	retBuf.append("\r\n");
	string str = "[" + sess->GetPlayerInfo().name + "] : " + sess->GetTcpSock().GetTotalBuf();
	retBuf.append( str );

	// retBuf 서버에 먼저 출력
	addrlen = sizeof(clientaddr);
	getpeername(sess->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[%s : %d]", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	printf("%s", retBuf.c_str());

	// 방에 있는 클라들에게 보내기
	retBuf.append("입력> ");
	sess->GetCurRoom()->SendData(sess, retBuf.c_str());

	// 채팅을 보낸 클라이언트 > 커서 다시 표시
	sess->GetTcpSock().Send("입력> ", strlen("입력> "));

	// 버퍼 비우기
	sess->GetTcpSock().SetBuf('\0');
	sess->GetTcpSock().GetTotalBuf().clear();
}
