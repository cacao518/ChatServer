#include "SessionManager.h"
#include "TcpSocket.h"
#include "Session.h"

///////////////////////////////////
/// @brief 巷充 敗呪 しいけしけい
///
///	@param	sock	更馬澗 痕呪
///	@param	sock	更馬澗 痕呪
//////////////////////////////////
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
		printf("[神嫌] 社掴 舛左研 蓄亜拝 呪 蒸柔艦陥!\n");
		return nullptr;
	}

	Session* ptr = new Session( TcpSocket(SocketType::TCP, IPType::IPv4) );

	if (ptr == NULL) {
		printf("[神嫌] 五乞軒亜 採膳杯艦陥!\n");
		return nullptr;
	}

	ptr->GetTcpSock().SetSocket(sock);
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

	if (ptr->GetCurRoom() != nullptr)
	{
		// Room拭 赤澗 member拭辞 薦暗馬壱, 号拭 赤澗 紫寓級拭惟 硝軒奄
		ptr->GetCurRoom()->LeaveRoom(client);

		// 羨紗曽戟 五獣走 号拭 赤澗 紫寓級拭惟 窒径
		string message = "\r\n		 * " + client->GetPlayerInfo().name + "還戚 " + "羨紗聖 曽戟梅柔艦陥.\r\n\n脊径> \0";
		ptr->GetCurRoom()->SendAllToRoomMembers(message.c_str());
	}

	// 社掴 丸奄
	closesocket(ptr->GetSock());
	
	// 室芝 set 拭辞 薦暗
	_clients.erase(client);

	client->SetIsExit(true);
	delete ptr;
}

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
	message.append("		" + name + " 政煽 舛左 \r\n=========================================================\r\n");
	message.append(" ID	戚硯		薄仙号 ID	薄仙 号 戚硯\r\n");
	message.append("---------------------------------------------------------\r\n");

	message.append(" " + to_string(id) + "	" + name + "			" + to_string(curRoomID) + "	" + curRoomName + "\r\n");

	message.append("\r\n");
	message.append("\r\n脊径> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

void SessionManager::ShowUserList(Session * sess)
{
	string message = "=========================================================\r\n		穿端 政煽 鯉系 \r\n=========================================================\r\n";
	message.append("ID	戚硯			\r\n");
	message.append("---------------------------------------------------------\r\n");

	for (auto client : _clients)
	{
		UINT id = client->GetPlayerInfo().id;
		string name = client->GetPlayerInfo().name;

		message.append(" " + to_string(id) + "	" + name + "\r\n");
	}

	message.append("\r\n");
	message.append("\r\n脊径> ");
	sess->GetTcpSock().Send(message.c_str());
}
