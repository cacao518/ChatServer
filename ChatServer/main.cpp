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

class TcpSocket;
class EndPoint;
int main(int argc, char* argv[])
{
	SessionManager sessMgr;
	ServerContext sc(sessMgr);

	if (sc.Init(SERVERPORT) != Error::None)
		return -1;

	sc.Run();
	
	return 0;
}
