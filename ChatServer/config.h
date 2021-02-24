#pragma once
#include <string>
using namespace std;

#define SERVERPORT 9000
#define BUFSIZE    512

struct PlayerInfo {
	int code;
	string name;
};

//enum class Result {
//	None,
//	SOCK_BIND_ERROR,
//	LISTEN_ERROR,
//	TCP_LISTEN_INVALID_SOCK_ERROR,
//	ACCEPT_ID_SOCKET,
//	SEND_ERROR,
//	RECV_ERROR
//};

enum class IPType {
	IPv4,
	IPv6
}; 
enum class SocketType {
	TCP,
	UDP,
};

struct ErrorUtil{

	// 소켓 함수 오류 출력 후 종료
	static void err_quit(const char* msg)
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
	static void err_display(const char* msg)
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
};