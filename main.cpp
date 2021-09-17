/***************************************/
/*  By Mezin Georgy  created: 16.09.21 */
/***************************************/

#include "Proxy.hpp"

void checkArgs(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "ERROR: Incorrect num of arguments.\nPlease, type ip-addr of BD-server, port of " <<\
			"BD-server and port of proxy-server:\n./proxy 127.0.0.1 3306 8080\n";
		exit(EXIT_FAILURE);
	}
	if (inet_addr(argv[1]) == INADDR_NONE)
	{
		std::cerr << "ERROR: Invalid ip-address of DB-server\n";
		exit(EXIT_FAILURE);
	}
}

void sigExit(int signum)
{
	if (SIGINT == signum ||
		signum == SIGTERM)
	{
		std::cout << "See you later!\n";
		_exit(EXIT_SUCCESS);
	}
}

int main(int argc, char **argv)
{
	signal(SIGTERM, sigExit);
	signal(SIGINT, sigExit);
	checkArgs(argc, argv);
	try
	{
		std::string ip = argv[1];
		int BDport = atoi(argv[2]);
		int proxyPort = atoi(argv[3]);
		if (BDport <= 0 || proxyPort <= 0)
			throw std::runtime_error("Invalid port\n");
		Proxy serv(ip, proxyPort, BDport);
		serv.listenSock(10);
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

