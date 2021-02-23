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
#include "ServerContext.h"
#include "Session.h"
#include "SessionManager.h"

int main(int argc, char* argv[])
{
	ServerContext sc;

	if (sc.Init(SERVERPORT) != Error::None)
		return -1;

	sc.Run();
	
	return 0;
}
