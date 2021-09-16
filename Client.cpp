#include "Client.hpp"

Client::Client(std::string &DBAddress, int fd, int DBport)
{
	_DBAddress = DBAddress;
	_clientFd = fd;
	_DBport = DBport;
	//test
		// _response = strdup("@1234");
	//
	_DBFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_DBFd < 0)
	{
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}
	_opt = 1;
	setsockopt(_DBFd, SOL_SOCKET, SO_REUSEADDR, &_opt, (socklen_t)(sizeof(_opt)));
	_addrLen = sizeof(_addr);
	bzero(&_addr, _addrLen);
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(DBport);
	if (!inet_aton(_DBAddress.c_str(), &(_addr.sin_addr)))
	{
		perror("Not valid ip-address");
		exit(EXIT_FAILURE);
	}
	if (connect(_DBFd, (struct sockaddr *)&_addr, (socklen_t)_addrLen))
	{
		perror("Failed connect to DB");
		exit (EXIT_FAILURE);
	}
	int flag = fcntl(_DBFd, F_GETFL);
	fcntl(_DBFd, F_SETFL, flag | O_NONBLOCK);
	std::cout << "New client has been connected to DB with socket " \
		<< _DBFd << " and client fd " << _clientFd << "\n";
	_status = READ_FROM_DB;
}
Client &Client::operator=(Client const &other)
{
	_clientFd = other._clientFd;
	_DBFd = other._DBFd;
	_DBport = other._DBport;
	_opt = other._DBport;
	_status = other._status;
	_addrLen = other._addrLen;
	_addr = other._addr;
	_DBAddress = other._DBAddress;
	memcpy(_request, other._request, sizeof(_request));
	memcpy(_response, other._response, sizeof(_response));
	_reqLen = other._reqLen;
	_resLen = other._resLen;
	return (*this);
}

Client::Client(){}
Client::~Client(){}

int Client::getClientFd()
{
	return (_clientFd);
}

int Client::getDBFd()
{
	return (_DBFd);
}

int Client::getStatus()
{
	return(_status);
}

void Client::setStatus(int status)
{
	_status = status;
}

void Client::reciveFromClient()
{
	std::cout << "wait from client\n";
	bzero(_request, BUFFSIZE);
	int len = recv(_clientFd, _request, sizeof(_request), MSG_NOSIGNAL);
	if (len < 0)
	{
		perror("recv failed()");
		exit (EXIT_FAILURE);
	}
	if (len == 0)
	{
		std::cout << "Connection with socket " << _clientFd << " was closed\n";
		_status = CLOSE;
		return ;
	}
	_reqLen = len;
	int pLen = int(int(_request[0])
		| int(_request[1]) << 8
		| int(_request[2]) << 16);
	std::cout << pLen << "\n";
	write(1, &_request[4], 1);
	write(1, "\n", 1);
	write(1, &_request[5], pLen - 1);
	write(1, "\n", 1);
	_status = SEND_TO_DB;
}

void Client::reciveFromDB()
{
	std::cout << "wait from DB\n";
	bzero(_response, BUFFSIZE);
	int len = recv(_DBFd, _response, sizeof(_response), 0);
	if (len < 0)
	{
		perror("recv failed()");
		exit (EXIT_FAILURE);
	}
	if (len == 0)
	{
		_status = CLOSE;
		return ;
	}
	_resLen = len;
	std::cout << len << "\n";
	write(1, _response, strlen((char*)_response));
	_status = SEND_TO_CLIENT;
}

void Client::sendToDB()
{
	std::cout << "send to DB\n";
	int len = send(_DBFd, _request, _reqLen, MSG_NOSIGNAL);
	if (len < 0)
	{
		perror("send() failed");
	}
	_status = READ_FROM_DB;
}

void Client::sendToClient()
{
	std::cout << "send to client\n";
	int len = send(_clientFd, _response, _resLen, MSG_NOSIGNAL);
	if (len < 0)
	{
		perror("send() failed");
	}
	_status = READ_FROM_CLIENT;
}