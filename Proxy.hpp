#ifndef PROXY_HPP
#define PROXY_HPP

#include "Client.hpp"
#include "ProxyServer.h"

class Proxy
{
private:
	int	_port;
	int _opt;
	int _DBport;
	std::string	_ipAddress;
	struct	sockaddr_in _addr;
	int	_addrLen;
	int	_fd;
	std::vector<Client> _clients;
	Proxy();
public:
	Proxy(std::string &ipAddress, int port, int DBport);
	int	getPort();
	int getAddrLen();
	int getFd();
	void listenSock(int backlog);
	void acceptClient();
	std::vector<Client> &getClients();
	void run();
	~Proxy();
};

#endif