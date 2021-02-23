#pragma once
#include <unordered_map>
#include "Session.h"

class PlayerInfo {
public:
	int code;
	std::string name;
};

class Room {





private:
	unordered_map<Session, PlayerInfo> members_;
};