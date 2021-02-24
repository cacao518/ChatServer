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

BOOL SessionManager::AddSession(SOCKET sock)
{
	if (clients_.size() >= FD_SETSIZE) {
		printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
		return FALSE;
	}

	Session* ptr = new Session( TcpSocket(SocketType::TCP, IPType::IPv4) );

	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return FALSE;
	}

	ptr->GetTcpSock().SetSocket(sock);
	ptr->GetTcpSock().SetRecvBytes(0);
	clients_.insert(ptr);

	return TRUE;
}


void SessionManager::RemoveSession(Session* client)
{
	if (clients_.empty()) return;

	Session* ptr = *(clients_.find(client));
	if (ptr == nullptr) return;

	// 클라이언트 정보 얻기
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->GetSock());
	clients_.erase(client);
	delete ptr;
}