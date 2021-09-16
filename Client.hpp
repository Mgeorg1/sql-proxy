#ifndef	CLIENT_HPP
#define	CLIENT_HPP
#define SEND_TO_DB 0
#define SEND_TO_CLIENT 1
#define READ_FROM_CLIENT 2
#define READ_FROM_DB 3
#define CLOSE 4
// #include "Proxy.hpp"
#include "ProxyServer.h"

class Client
{
private:
	int _clientFd;
	int _DBFd;
	int _DBport;
	int _opt;
	int _status;
	int _addrLen;
	struct sockaddr_in _addr;
	std::string _DBAddress;
	char _request[BUFFSIZE];
	char _response[BUFFSIZE];
	int _reqLen;
	int _resLen;
	Client();

public:
	Client(std::string &DBAddress, int fd, int DBport);
	~Client();
	void setStatus(int status);
	// void setRequest(char *req);
	// void setResponse(char *res)
	int getStatus();
	int getClientFd();
	int getDBFd();
	void reciveFromClient();
	void sendToClient();
	void reciveFromDB();
	void sendToDB();
	Client &operator=(Client const &other);

};

#endif