#pragma once
#include <string>
using namespace std;

//#define SERVERPORT 9000
//#define BUFSIZE    512
//#define ROOM_USER_MAX 100

constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 512;
constexpr int ROOM_USER_MAX = 100;

constexpr int CHAT_BUF_MAX = 100;
constexpr int COMMAND_MAX = 10;


enum class PacketKind {
	Login,				//		�α��� : /login �г���
	Help,				//		��ɾ� �ȳ� : /help
	Exit,				//		���� : /exit
	ShowAllUser,		//		��ü ���� ��� : /all user
	ShowAllRoom,		//		��ü �� ��� : /all room
	ShowUser,			//		�� ���� ��� : /user
	ShowRoomInfo,		//		���� �� ���� ��� : /r ���ȣ
	ShowUserInfo,		//		���� ���� ���� ��� : /i �̸�
	Whisper,			//		�ӼӸ� : /w �̸� 
	MakeRoom,			//		�游��� : /make �ִ��ο� ������
	JoinRoom,			//		�� ���� : /join ���ȣ  
	End
};

struct PlayerInfo {
	int code;
	string name;
};

struct RoomInfo {
	int _roomCode;
	int _memberNumMax;
	string _roomName = "unknown";

	bool _isLobby = false;
};

enum class IPType {
	IPv4,
	IPv6
}; 
enum class SocketType {
	TCP,
	UDP,
};

struct ErrorUtil{

	// ���� �Լ� ���� ��� �� ����
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

	// ���� �Լ� ���� ���
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