/***************************************/
/*  By Mezin Georgy  created: 16.09.21 */
/***************************************/

#include "Proxy.hpp"

Proxy::Proxy(std::string &ipAddress, int port, int DBport)
{
	_ipAddress = ipAddress;
	_port = port;
	_DBport = DBport;
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
	{
		perror("socket failed()");
		exit (EXIT_FAILURE);
	}
	_opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, (socklen_t)(sizeof(_opt)));
	_addrLen = sizeof(_addr);
	bzero(&_addr, _addrLen);
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(port);
	if (bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
	{
		perror("bind() failed");
		close(_fd);
		exit(EXIT_FAILURE);
	}
	int flag = fcntl(_fd, F_GETFL);
	fcntl(_fd, F_SETFL, flag | O_NONBLOCK);
	std::cout << "Proxy-server with port " << _port << " created\n";
}

Proxy::~Proxy()
{
	close(_fd);
	for (std::vector<Client>::iterator it = getClients().begin();
		it != getClients().end(); ++it)
	{
		close(it->getClientFd());
		close(it->getDBFd());
	}
	std::cout << "Closed all connections\n";
}

Proxy::Proxy()
{
}

int	Proxy::getFd()
{
	return (_fd);
}

int Proxy::getAddrLen()
{
	return (_addrLen);
}

int Proxy::getPort()
{
	return (_port);
}

void Proxy::listenSock(int backlog)
{
	if (listen(_fd, backlog) < 0)
	{
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "Server is listening on port " << _port << "\n";
}

void Proxy::acceptClient()
{
	int newSocket = accept(_fd, (struct sockaddr *)&_addr,
						   (socklen_t *)&_addrLen);
	if (newSocket < 0)
		throw std::runtime_error(std::string("Accept failed()" + std::string(strerror(errno))));
	// setsockopt(newSocket, SOL_SOCKET, SO_NOSIGPIPE, &_opt, (socklen_t)sizeof(_opt));
	setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &_opt, (socklen_t)(sizeof(_opt)));
	int flag = fcntl(newSocket, F_GETFL);
	fcntl(newSocket, F_SETFL, flag |  O_NONBLOCK);
	_clients.push_back(Client(_ipAddress, newSocket, _DBport));

	std::cout << "New connection with client " << inet_ntoa(_addr.sin_addr) << ":"<< ntohs(_addr.sin_port) << "\n";
}

std::vector<Client> &Proxy::getClients()
{
	return (_clients);
}

void Proxy::run()
{
	fd_set writeFds, readFds;
	int maxFd = _fd;
	int clientFd, DBFd;
	int res;
	while (true)
	{
		FD_ZERO(&writeFds);
		FD_ZERO(&readFds);
		FD_SET(_fd, &readFds);
		for (std::vector<Client>::iterator client = _clients.begin();
			 client != _clients.end(); ++client)
		{
			clientFd = client->getClientFd();
			DBFd = client->getDBFd();
			if (client->getStatus() == READ_FROM_CLIENT)
				FD_SET(clientFd, &readFds);
			else if (client->getStatus() == READ_FROM_DB)
				FD_SET(DBFd, &readFds);
			else if (client->getStatus() == SEND_TO_CLIENT)
				FD_SET(clientFd, &writeFds);
			else if (client->getStatus() == SEND_TO_DB)
				FD_SET(DBFd, &writeFds);
			else if (client->getStatus() == CLOSE)
			{
				FD_CLR(clientFd, &writeFds);
				FD_CLR(clientFd, &readFds);
				FD_CLR(DBFd, &writeFds);
				FD_CLR(DBFd, &readFds);
				// close(DBFd); //не получается закрывать дескрипторы при использовании в качестве клиента
				// DBeaver: при обновлении соединения возникает ошибка на селекте bad file descriptor
				// close(clientFd);
				continue ;
			}
			if (clientFd > maxFd)
				maxFd = clientFd;
			if (DBFd > maxFd)
				maxFd = DBFd;
		}
		res = select(maxFd + 1, &readFds, &writeFds, NULL, NULL);
		if (res == -1 && errno != EINTR)
		{
			std::string error = "ERROR: select() failed: " + std::string(strerror(errno));
			throw std::runtime_error(error);
		}

		if (FD_ISSET(_fd, &readFds))
		try
		{
			acceptClient();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			if (std::string(e.what()) == "Failed connect to DB")
				throw std::exception(); 
			continue ;
		}
		for (std::vector<Client>::iterator client = _clients.begin();
			 client != _clients.end(); ++client)
		{
			clientFd = client->getClientFd();
			DBFd = client->getDBFd();
			try
			{
				if (FD_ISSET(clientFd, &readFds))
					client->reciveFromClient();
				if (FD_ISSET(DBFd, &readFds))
					client->reciveFromDB();
				if (FD_ISSET(clientFd, &writeFds))
					client->sendToClient();
				if (FD_ISSET(DBFd, &writeFds))
					client->sendToDB();
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
		}
	}
}