#include "RoomManager.h"
#include "Room.h"

RoomManager* RoomManager::instance = nullptr;

RoomManager* RoomManager::GetInstance()
{
	if (instance == nullptr)
		instance = new RoomManager();
	
	return instance;

}

/// �� �����
Room* RoomManager::AddRoom(Session* master, int max, string name)
{
	UINT id = GetNewRoomID();

	// ������ ������ �κ�, ������ ������ �Ϲ� �� ����
	if (master == nullptr)
		_rooms[id] = new Room(id); // �κ�
	else
		_rooms[id] = new Room(master, id, max, name); //������ ���� ��

	return _rooms[id];
}

/// �� ����
void RoomManager::RemoveRoom(Room * room)
{
	// (1) �濡 �ִ� ���� �� ������ ����
	for (auto client : room->GetMembers())
	{
		if (client == nullptr) continue;
		room->LeaveRoom(client);			// �濡�� ������
		client->SetCurRoom(_rooms[1]);		// ���� �� �κ�� ����
		_rooms[1]->EnterRoom(client);		// �κ� ����

	}

	// (2) �� ��ϵ� �����Ѵ�.
	if (_rooms.find(room->GetRoomInfo()._roomID) != _rooms.end())
		_rooms.erase(room->GetRoomInfo()._roomID);
	
}

/// ������ �� ���� ���
BOOL RoomManager::ShowRoomInfo(Session * sess, UINT roomID)
{
	if (_rooms.find(roomID) == _rooms.end()) return FALSE; // ã�� ���� ������ ����
	Room* room = _rooms.find(roomID)->second;
	if (room == nullptr) return FALSE;					// ã�Ҵµ� ���� nullptr�� ����

	string name = room->GetRoomInfo()._roomName;
	string message = "=========================================================\r\n";
	message.append("		" + name + " �� ���� \r\n=========================================================\r\n");
	message.append(" ID	�̸�			����\r\n");
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

		// �𸮾� ����
		sendData_unreal.append(to_string(id) + "(" + name + ")");
	}

	// �𸮾� ���� ��Ŷ
	string message_unreal = to_string((int)PacketKind::ShowRoomInfo) + '{' + sendData_unreal + '}';
	if (sess->GetIsUnreal()) sess->GetTcpSock().Send(message_unreal.c_str());


	message.append("\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());

	return TRUE;
}

/// ��ü �� ��� ���
void RoomManager::ShowRoomList(Session * sess)
{
	string message = "=========================================================\r\n		��ü �� ��� \r\n=========================================================\r\n";
	message.append("��ȣ	�̸�			�����ο���\r\n");
	message.append("---------------------------------------------------------\r\n");

	string sendData_unreal;
	for (auto room : _rooms)
	{
		UINT id = room.second->GetRoomInfo()._roomID;
		string name = room.second->GetRoomInfo()._roomName;
		int userNum = room.second->GetMembers().size();

		message.append(" " + to_string(id) + "	" + name + "				"  + to_string(userNum) + "\r\n");


		// �𸮾� ����
		sendData_unreal.append(to_string(id) + "(" + name + ")" + "(" + to_string(userNum) + ")");
	}

	// �𸮾� ���� ��Ŷ
	string message_unreal = to_string((int)PacketKind::ShowRoom) + '{' + sendData_unreal + '}';
	if (sess->GetIsUnreal()) sess->GetTcpSock().Send(message_unreal.c_str());


	message.append("\r\n");
	message.append("\r\n�Է�> ");
	sess->GetTcpSock().Send(message.c_str());
}
