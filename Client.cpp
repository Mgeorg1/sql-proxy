
/***************************************/
/*  By Mezin Georgy  created: 16.09.21 */
/***************************************/

#include "Client.hpp"

Client::Client(std::string &DBAddress, int fd, int DBport)
{
	_DBAddress = DBAddress;
	_clientFd = fd;
	_DBport = DBport;
	_DBFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_DBFd < 0)
		throw std::runtime_error("socket() failed");
	_opt = 1;
	setsockopt(_DBFd, SOL_SOCKET, SO_REUSEADDR, &_opt, (socklen_t)(sizeof(_opt)));
	_addrLen = sizeof(_addr);
	bzero(&_addr, _addrLen);
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(DBport);
	if (!inet_aton(_DBAddress.c_str(), &(_addr.sin_addr)))
		throw std::runtime_error("Not valid ip-address");
	if (connect(_DBFd, (struct sockaddr *)&_addr, (socklen_t)_addrLen))
		throw std::runtime_error("Failed connect to DB");
	int flag = fcntl(_DBFd, F_GETFL);
	fcntl(_DBFd, F_SETFL, flag | O_NONBLOCK);
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
	bzero(_request, BUFFSIZE);
	int len = recv(_clientFd, _request, sizeof(_request), MSG_NOSIGNAL);
	if (len < 0)
	{
		std::string error = "ERROR: recv() failed: " + std::string(strerror(errno));
		_status = CLOSE;
		throw std::runtime_error(error);
	}
	if (len == 0)
	{
		std::cout << "Connection with socket was closed\n";
		_status = CLOSE;
		return ;
	}
	_reqLen = len;
	if (int(_request[4]) == COM_QUERY || int(_request[4]) == COM_STM_PREPARE)
	{
		try
		{
			logger(_request);
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	_status = SEND_TO_DB;
}

void Client::reciveFromDB()
{
	bzero(_response, BUFFSIZE);
	int len = recv(_DBFd, _response, sizeof(_response), 0);
	if (len < 0)
	{
		std::string error = "ERROR: recv() failed: " + std::string(strerror(errno));
		_status = CLOSE;
		throw std::runtime_error(error);
	}
	if (len == 0)
	{
		std::cout << "Connection was closed\n";
		_status = CLOSE;
		return ;
	}
	_resLen = len;
	_status = SEND_TO_CLIENT;
}

void Client::sendToDB()
{
	int len = send(_DBFd, _request, _reqLen, MSG_NOSIGNAL);
	if (len < 0)
	{
		std::string error = "ERROR: send() failed: " + std::string(strerror(errno));
		_status = CLOSE;
		throw std::runtime_error(error);
	}
	_status = READ_FROM_DB;
}

void Client::sendToClient()
{
	int len = send(_clientFd, _response, _resLen, MSG_NOSIGNAL);
	if (len < 0)
	{
		std::string error = "ERROR: send() failed: " + std::string(strerror(errno));
		_status = CLOSE;
		throw std::runtime_error(error);
	}
	_status = READ_FROM_CLIENT;
}