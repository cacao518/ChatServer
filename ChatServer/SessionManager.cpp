#include "SessionManager.h"
#include "TcpSocket.h"
#include "Session.h"


SessionManager* SessionManager::instance = nullptr;

SessionManager* SessionManager::GetInstance()
{
	if (instance == NULL)
		instance = new SessionManager();

	return instance;

}

///////////////////////////////////
/// @brief 무슨 함수 ㅇㄴㅁㅇㅁㄴ
///
///	@param	sock	뭐하는 변수
///	@param	sock	뭐하는 변수
//////////////////////////////////
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
	ptr->GetTcpSock().SetRecvBytes(0);
	_clients.insert(ptr);

	// 방금 추가한 세션 반환
	return ptr;
}


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

	// Room에 있는 member에서 제거하고, 방에 있는 사람들에게 알리기
	ptr->GetCurRoom()->LeaveRoom(client);

	// 접속종료 메시지 방에 있는 사람들에게 출력
	string message = "\r\n		 * " + client->GetPlayerInfo().name + "님이 " + "접속을 종료했습니다.\r\n\n입력> \0";
	ptr->GetCurRoom()->SendAllToRoomMembers(message.c_str());

	// 소켓 닫기
	closesocket(ptr->GetSock());
	
	// 세션 set 에서 제거
	_clients.erase(client);
	delete ptr;
}