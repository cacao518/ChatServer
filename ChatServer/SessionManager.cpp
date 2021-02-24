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

Session* SessionManager::AddSession(SOCKET sock)
{
	if (_clients.size() >= FD_SETSIZE) {
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return nullptr;
	}

	Session* ptr = new Session( TcpSocket(SocketType::TCP, IPType::IPv4) );

	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return nullptr;
	}

	ptr->GetTcpSock().SetSocket(sock);
	ptr->GetTcpSock().SetRecvBytes(0);
	_clients.insert(ptr);

	// ��� �߰��� ���� ��ȯ
	return ptr;
}


void SessionManager::RemoveSession(Session* client)
{
	if (_clients.empty()) return;

	Session* ptr = *(_clients.find(client));
	if (ptr == nullptr) return;

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSock(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->GetSock());
	_clients.erase(client);
	delete ptr;
}