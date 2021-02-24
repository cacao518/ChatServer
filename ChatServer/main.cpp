#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <set>

#include "ServerContext.h"

int main(int argc, char* argv[])
{
	ServerContext sc;

	if (sc.Init(SERVERPORT) == FALSE)
		return -1;

	sc.Run();
	
	return 0;
}
