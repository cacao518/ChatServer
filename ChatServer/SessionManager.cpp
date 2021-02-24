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
/// @brief 巷充 敗呪 しいけしけい
///
///	@param	sock	更馬澗 痕呪
///	@param	sock	更馬澗 痕呪
///	@param	sock	更馬澗 痕呪
//////////////////////////////////
Session* SessionManager::AddSession(SOCKET sock)
{
	if (_clients.size() >= FD_SETSIZE) {
		printf("[神嫌] 社掴 舛左研 蓄亜拝 呪 蒸柔艦陥!\n");
		return nullptr;
	}

	Session* ptr = new Session( TcpSocket(SocketType::TCP, IPType::IPv4) );

	if (ptr == NULL) {
		printf("[神嫌] 五乞軒亜 採膳杯艦陥!\n");
		return nullptr;
	}

	ptr->GetTcpSock().SetSocket(sock);
	ptr->GetTcpSock().SetRecvBytes(0);
	_clients.insert(ptr);

	// 号榎 蓄亜廃 室芝 鋼発
	return ptr;
}


void SessionManager::RemoveSession(Session* client)
{
	if (_clients.empty()) return;

	Session* ptr = *(_clients.find(client));
	if (ptr == nullptr) return;

	// 適虞戚情闘 舛左 条奄
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP 辞獄] 適虞戚情闘 曽戟: IP 爽社=%s, 匂闘 腰硲=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->GetSock());
	_clients.erase(client);
	delete ptr;
}