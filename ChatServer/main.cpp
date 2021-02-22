#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <set>

#include "EndPoint.h"
#include "TcpSocket.h"
#include "config.h"

#define SERVERPORT 9000
#define BUFSIZE    512

class TcpSocket;
class EndPoint;

set<TcpSocket*> tcpSocketList;

// ���� ���� �Լ�
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(TcpSocket* tcpSock);

// ���� ��� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);

int main(int argc, char* argv[])
{
	int retval;

	TcpSocket listen_sock(SocketType::TCP, IPType::IPv4);
	listen_sock.Bind(EndPoint("127.0.0.1", SERVERPORT, IPType::IPv4));
	listen_sock.Listen();

	// ������ ��ſ� ����� ����
	FD_SET rset, wset;
	TcpSocket client_socket;
	SOCKADDR_IN clientaddr;
	int addrlen;

	while (1) {
		// ���� �� �ʱ�ȭ
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock.GetSocket(), &rset);

		for (auto it = tcpSocketList.begin(); it != tcpSocketList.end(); it++)
		{
			// ���͸� ������� ���� �¿� �߰�
			if ((*it)->GetBuf() == '\n')
				FD_SET((*it)->GetSocket(), &wset);
			else
				FD_SET((*it)->GetSocket(), &rset);
		}

		// select()
		retval = select(0, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR) err_quit("select()");

		// ���� �� �˻�(1): Ŭ���̾�Ʈ ���� ����
		if (FD_ISSET(listen_sock.GetSocket(), &rset)) 
		{
			if(listen_sock.Accept(client_socket) == INVALID_SOCKET)
			{
				err_display("accept()");
			}
			else 
			{
				printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
					client_socket.GetIPAddress().c_str(), client_socket.GetPort());
				// ���� ���� �߰�
				AddSocketInfo(client_socket.GetSocket());

				char ss[50] = "ä�� �κ� �����ϼ̽��ϴ�.\r\n\n>\0";
				retval = send(client_socket.GetSocket(), ss, strlen(ss), 0);
			}
		}

		// ���� �� �˻�(2): ������ ���
		for (auto iter = tcpSocketList.begin(); iter != tcpSocketList.end(); iter++)
		{
			TcpSocket* ptr = *iter;
			char retBuf[BUFSIZE];
			if (FD_ISSET(ptr->GetSocket(), &rset)) 
			{
				// ������ �ޱ�
				retval = recv(ptr->GetSocket(), &(ptr->GetBuf()), sizeof(char), 0); // �ѱ��ڸ� �޾�
				if (retval == SOCKET_ERROR) {
					err_display("recv()");
					RemoveSocketInfo(ptr);
					break;
				}
				else if (retval == 0) {
					RemoveSocketInfo(ptr);
					break;
				}
				ptr->GetTotalBuf().push_back(ptr->GetBuf());
			}
			if (FD_ISSET(ptr->GetSocket(), &wset))
			{
				// ������ ������
				int idx = 0;
				retBuf[idx++] = '\r';
				retBuf[idx++] = '\n';

				for (auto it = ptr->GetTotalBuf().begin(); it != ptr->GetTotalBuf().end(); it++)
				{
					retBuf[idx++] = *it;
				}
				
				retBuf[idx] = '\0';

				// retBuf ������ ���� ���
				addrlen = sizeof(clientaddr);
				getpeername(ptr->GetSocket(), (SOCKADDR*)&clientaddr, &addrlen);
				printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf);


				// Ŭ��鿡�� ������
				retBuf[idx++] = '>';
				retBuf[idx] = '\0';
				for (auto it = tcpSocketList.begin(); it != tcpSocketList.end(); it++)
				{
					// ä���� ���� Ŭ���̾�Ʈ�� ����
					if ((*it)->GetSocket() == ptr->GetSocket())
						continue;

					retval = send((*it)->GetSocket(), retBuf, ptr->GetTotalBuf().size() + 4, 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
						RemoveSocketInfo(ptr);
						break;
					}
				}

				// ä���� ���� Ŭ���̾�Ʈ > Ŀ�� �ٽ� ǥ��
				char ss[50] = ">\0";
				retval = send(ptr->GetSocket(), ss, strlen(ss), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					RemoveSocketInfo(ptr);
					break;
				}

				// ���� ����
				ptr->SetBuf('\0');
				ptr->GetTotalBuf().clear();
			}
		}
	}

	// ���� ����
	WSACleanup();
	return 0;
}

// ���� ���� �߰�
BOOL AddSocketInfo(SOCKET sock)
{
	if (tcpSocketList.size() >= FD_SETSIZE) {
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return FALSE;
	}

	//SOCKETINFO* ptr = new SOCKETINFO;
	TcpSocket* ptr = new TcpSocket(SocketType::TCP, IPType::IPv4);
	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return FALSE;
	}

	ptr->SetSocket(sock);
	ptr->SetRecvBytes(0);
	tcpSocketList.insert(ptr);

	return TRUE;
}

// ���� ���� ����
void RemoveSocketInfo(TcpSocket* tcpSock)
{
	TcpSocket* ptr = *(tcpSocketList.find(tcpSock));
	if (ptr == nullptr) return;

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSocket(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->GetSocket());
	tcpSocketList.erase(tcpSock);
	delete ptr;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}