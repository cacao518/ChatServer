#include "SessionManager.h"
#include "TcpSocket.h"
#include "Session.h"

SessionManager* SessionManager::instance = nullptr;

SessionManager* SessionManager::GetInstance()
{
	if (instance == nullptr)
		instance = new SessionManager();

	return instance;

}

/// 세션(클라이언트) 추가
Session* SessionManager::AddSession(SOCKET sock)
{
	if (_clients.size() >= FD_SETSIZE) {
		printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
		return nullptr;
	}

	Session* ptr = new Session( TcpSocket(SocketType::TCP, IPType::IPv4) );

	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return nullptr;
	}

	ptr->GetTcpSock().SetSocket(sock);
	_clients.insert(ptr);

	// 방금 추가한 세션 반환
	return ptr;
}


/// 세션(클라이언트) 제거
void SessionManager::RemoveSession(Session* client)
{
	if (_clients.empty()) return;

	Session* ptr = *(_clients.find(client));
	if (ptr == nullptr) return;

	// 클라이언트 정보 얻기
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	if (ptr->GetCurRoom() != nullptr)
	{
		// Room에 있는 member에서 제거하고, 방에 있는 사람들에게 알리기
		ptr->GetCurRoom()->LeaveRoom(client);

		// 접속종료 메시지 방에 있는 사람들에게 출력
		string message = "\r\n		 * " + client->GetPlayerInfo().name + "님이 " + "접속을 종료했습니다.\r\n\n입력> \0";
		ptr->GetCurRoom()->SendAllToRoomMembers(message.c_str());
	}

	// 소켓 닫기
	closesocket(ptr->GetSock());
	
	// 세션 set 에서 제거
	_clients.erase(client);

	client->SetIsExit(true);
	delete ptr; // 메모리 해제
}

/// 선택한 유저 정보 보기
BOOL SessionManager::ShowUserInfo(Session * sess, string name)
{
	UINT id;
	UINT curRoomID;
	string curRoomName;
	bool isFindSuccess = false;
	for (auto client : _clients)
	{
		if (client->GetPlayerInfo().name == name)
		{
			if (client->GetIsLogin() == false) return FALSE;
			id = client->GetPlayerInfo().id;
			curRoomID = client->GetCurRoom()->GetRoomInfo()._roomID;
			curRoomName = client->GetCurRoom()->GetRoomInfo()._roomName;
			isFindSuccess = true;
			break;
		}
	}
	if (isFindSuccess == false) return FALSE;
	string message = "=========================================================\r\n";
	message.append("		" + name + " 유저 정보 \r\n=========================================================\r\n");
	message.append(" ID	이름		현재방 ID	현재 방 이름\r\n");
	message.append("---------------------------------------------------------\r\n");

	message.append(" " + to_string(id) + "	" + name + "			" + to_string(curRoomID) + "	" + curRoomName + "\r\n");

	message.append("\r\n");
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// 모든 유저 리스트 보기
void SessionManager::ShowUserList(Session * sess)
{
	string message = "=========================================================\r\n		전체 유저 목록 \r\n=========================================================\r\n";
	message.append("ID	이름			\r\n");
	message.append("---------------------------------------------------------\r\n");

	for (auto client : _clients)
	{
		if (client->GetIsLogin() == false) continue;
		UINT id = client->GetPlayerInfo().id;
		string name = client->GetPlayerInfo().name;

		message.append(" " + to_string(id) + "	" + name + "\r\n");
	}

	message.append("\r\n");
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());
}
