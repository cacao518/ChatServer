#include "RoomManager.h"
#include "Room.h"

RoomManager* RoomManager::instance = nullptr;

RoomManager* RoomManager::GetInstance()
{
	if (instance == nullptr)
		instance = new RoomManager();
	
	return instance;

}

/// 방 만들기
Room* RoomManager::AddRoom(Session* master, int max, string name)
{
	UINT id = GetNewRoomID();

	// 방장이 없으면 로비, 방장을 받으면 일반 방 생성
	if (master == nullptr)
		_rooms[id] = new Room(id); // 로비
	else
		_rooms[id] = new Room(master, id, max, name); //유저가 만든 방

	return _rooms[id];
}

/// 방 삭제
void RoomManager::RemoveRoom(Room * room)
{
	// (1) 방에 있는 유저 다 내보낸 다음
	for (auto client : room->GetMembers())
	{
		if (client == nullptr) continue;
		room->LeaveRoom(client);			// 방에서 나오기
		client->SetCurRoom(_rooms[1]);		// 현재 방 로비로 설정
		_rooms[1]->EnterRoom(client);		// 로비 입장

	}

	// (2) 방 목록도 삭제한다.
	if (_rooms.find(room->GetRoomInfo()._roomID) != _rooms.end())
		_rooms.erase(room->GetRoomInfo()._roomID);
	
}

/// 선택한 방 정보 출력
BOOL RoomManager::ShowRoomInfo(Session * sess, UINT roomID)
{
	if (_rooms.find(roomID) == _rooms.end()) return FALSE; // 찾는 방이 없으면 리턴
	Room* room = _rooms.find(roomID)->second;
	if (room == nullptr) return FALSE;					// 찾았는데 방이 nullptr라도 리턴

	string name = room->GetRoomInfo()._roomName;
	string message = "=========================================================\r\n";
	message.append("		" + name + " 방 정보 \r\n=========================================================\r\n");
	message.append(" ID	이름			방장\r\n");
	message.append("---------------------------------------------------------\r\n");

	string sendData_unreal;
	for (auto client : room->GetMembers())
	{
		bool isMaster = false;
		Session* master = room->GetMaster();
		if (client == master) isMaster = true;

		UINT id = client->GetPlayerInfo().id;
		string name = client->GetPlayerInfo().name;

		if(isMaster) 
			message.append(to_string(id) + "	" + name + "			Master" + "\r\n");
		else
			message.append(to_string(id) + "	" + name + "\r\n");

		// 언리얼 전용
		sendData_unreal.append(to_string(id) + "(" + name + ")");
	}

	// 언리얼 전용 패킷
	string message_unreal = to_string((int)PacketKind::ShowRoomInfo) + '{' + sendData_unreal + '}';
	if (sess->GetIsUnreal()) sess->GetTcpSock().Send(message_unreal.c_str());


	message.append("\r\n");
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// 전체 방 목록 출력
void RoomManager::ShowRoomList(Session * sess)
{
	string message = "=========================================================\r\n		전체 방 목록 \r\n=========================================================\r\n";
	message.append("번호	이름			현재인원수\r\n");
	message.append("---------------------------------------------------------\r\n");

	string sendData_unreal;
	for (auto room : _rooms)
	{
		UINT id = room.second->GetRoomInfo()._roomID;
		string name = room.second->GetRoomInfo()._roomName;
		int userNum = room.second->GetMembers().size();

		message.append(" " + to_string(id) + "	" + name + "				"  + to_string(userNum) + "\r\n");


		// 언리얼 전용
		sendData_unreal.append(to_string(id) + "(" + name + ")" + "(" + to_string(userNum) + ")");
	}

	// 언리얼 전용 패킷
	string message_unreal = to_string((int)PacketKind::ShowRoom) + '{' + sendData_unreal + '}';
	if (sess->GetIsUnreal()) sess->GetTcpSock().Send(message_unreal.c_str());


	message.append("\r\n");
	message.append("\r\n입력> ");
	sess->GetTcpSock().Send(message.c_str());
}
