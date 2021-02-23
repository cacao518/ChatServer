#include "Session.h"

Session::Session(TcpSocket& socket)
{
	socket_ = socket;
}

Session::~Session()
{
}

void Session::Start()
{
}
