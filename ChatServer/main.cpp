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

// 소켓 관리 함수
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(TcpSocket* tcpSock);

// 오류 출력 함수
void err_quit(const char* msg);
void err_display(const char* msg);

int main(int argc, char* argv[])
{
	int retval;

	TcpSocket listen_sock(SocketType::TCP, IPType::IPv4);
	listen_sock.Bind(EndPoint("127.0.0.1", SERVERPORT, IPType::IPv4));
	listen_sock.Listen();

	// 데이터 통신에 사용할 변수
	FD_SET rset, wset;
	TcpSocket client_socket;
	SOCKADDR_IN clientaddr;
	int addrlen;

	while (1) {
		// 소켓 셋 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock.GetSocket(), &rset);

		for (auto it = tcpSocketList.begin(); it != tcpSocketList.end(); it++)
		{
			// 엔터를 쳤을경우 쓰기 셋에 추가
			if ((*it)->GetBuf() == '\n')
				FD_SET((*it)->GetSocket(), &wset);
			else
				FD_SET((*it)->GetSocket(), &rset);
		}

		// select()
		retval = select(0, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR) err_quit("select()");

		// 소켓 셋 검사(1): 클라이언트 접속 수용
		if (FD_ISSET(listen_sock.GetSocket(), &rset)) 
		{
			if(listen_sock.Accept(client_socket) == INVALID_SOCKET)
			{
				err_display("accept()");
			}
			else 
			{
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", 
					client_socket.GetIPAddress().c_str(), client_socket.GetPort());
				// 소켓 정보 추가
				AddSocketInfo(client_socket.GetSocket());

				char ss[50] = "채팅 로비에 입장하셨습니다.\r\n\n>\0";
				retval = send(client_socket.GetSocket(), ss, strlen(ss), 0);
			}
		}

		// 소켓 셋 검사(2): 데이터 통신
		for (auto iter = tcpSocketList.begin(); iter != tcpSocketList.end(); iter++)
		{
			TcpSocket* ptr = *iter;
			char retBuf[BUFSIZE];
			if (FD_ISSET(ptr->GetSocket(), &rset)) 
			{
				// 데이터 받기
				retval = recv(ptr->GetSocket(), &(ptr->GetBuf()), sizeof(char), 0); // 한글자만 받아
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
				// 데이터 보내기
				int idx = 0;
				retBuf[idx++] = '\r';
				retBuf[idx++] = '\n';

				for (auto it = ptr->GetTotalBuf().begin(); it != ptr->GetTotalBuf().end(); it++)
				{
					retBuf[idx++] = *it;
				}
				
				retBuf[idx] = '\0';

				// retBuf 서버에 먼저 출력
				addrlen = sizeof(clientaddr);
				getpeername(ptr->GetSocket(), (SOCKADDR*)&clientaddr, &addrlen);
				printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retBuf);


				// 클라들에게 보내기
				retBuf[idx++] = '>';
				retBuf[idx] = '\0';
				for (auto it = tcpSocketList.begin(); it != tcpSocketList.end(); it++)
				{
					// 채팅을 보낸 클라이언트는 제외
					if ((*it)->GetSocket() == ptr->GetSocket())
						continue;

					retval = send((*it)->GetSocket(), retBuf, ptr->GetTotalBuf().size() + 4, 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
						RemoveSocketInfo(ptr);
						break;
					}
				}

				// 채팅을 보낸 클라이언트 > 커서 다시 표시
				char ss[50] = ">\0";
				retval = send(ptr->GetSocket(), ss, strlen(ss), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					RemoveSocketInfo(ptr);
					break;
				}

				// 버퍼 비우기
				ptr->SetBuf('\0');
				ptr->GetTotalBuf().clear();
			}
		}
	}

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 소켓 정보 추가
BOOL AddSocketInfo(SOCKET sock)
{
	if (tcpSocketList.size() >= FD_SETSIZE) {
		printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
		return FALSE;
	}

	//SOCKETINFO* ptr = new SOCKETINFO;
	TcpSocket* ptr = new TcpSocket(SocketType::TCP, IPType::IPv4);
	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return FALSE;
	}

	ptr->SetSocket(sock);
	ptr->SetRecvBytes(0);
	tcpSocketList.insert(ptr);

	return TRUE;
}

// 소켓 정보 삭제
void RemoveSocketInfo(TcpSocket* tcpSock)
{
	TcpSocket* ptr = *(tcpSocketList.find(tcpSock));
	if (ptr == nullptr) return;

	// 클라이언트 정보 얻기
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->GetSocket(), (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->GetSocket());
	tcpSocketList.erase(tcpSock);
	delete ptr;
}

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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