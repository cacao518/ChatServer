#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <set>

#include "ServerContext.h"

int main()
{
	ServerContext sc;

	sc.Init(SERVERPORT);
	sc.Run();
	
	return 0;
}
