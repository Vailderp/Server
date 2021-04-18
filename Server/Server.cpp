#include "Server.h"

int main(int argc, char* argv[])
{
	vl::Server server(1 << 16, 1111);
	
	server.onAny([&](const char* packet, const SOCKET socket)
		{
		
			server.emit(packet, socket);
		
		});
	
	server.start();
	
	system("pause");
	return EXIT_SUCCESS;
}

