#include "Proxy.hpp"

int main()
{
	std::string ip = "127.0.0.1";
	Proxy serv(ip, 8080, 3306);
	serv.listenSock(10);
	// serv.acceptClient();
	serv.run();
	// Client &client = serv.getClients().back();
	// while(1)
	// {
	// 	std::cout << client.getClientFd() << "\n";
	// 	std::cout << client.getStatus() << "\n";
	// 	std::cout << client.getDBFd() << "\n";
	// 	client.reciveFromDB();
	// 	client.sendToClient();
	// 	client.reciveFromClient();
	// 	client.sendToDB();
	// }
}

