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
	Login,				//		로그인 : /login 닉네임
	Help,				//		명령어 안내 : /help
	Exit,				//		종료 : /exit
	ShowAllUser,		//		전체 유저 출력 : /all user
	ShowAllRoom,		//		전체 방 출력 : /all room
	ShowUser,			//		방 유저 출력 : /user
	ShowRoomInfo,		//		선택 방 정보 출력 : /r 방번호
	ShowUserInfo,		//		선택 유저 정보 출력 : /i 이름
	Whisper,			//		귓속말 : /w 이름 
	MakeRoom,			//		방만들기 : /make 최대인원 방제목
	JoinRoom,			//		방 참여 : /join 방번호  
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