#include "Session.h"

Session::Session(TcpSocket& socket)
{
	socket_ = socket;
	socket_.SetSession(this);
}

Session::~Session()
{
}

void Session::Start()
{
}
