/***************************************/
/*  By Mezin Georgy  created: 16.09.21 */
/***************************************/

#include "ProxyServer.h"
#include <ctime>

void logger(char *packet)
{
	char *date;
	time_t now;
	now = time(0);
	date = ctime(&now);
	int fd = open("proxy.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd < 0)
	{
		std::string error = "CANNOT OPEN LOG FILE: " + std::string(strerror(errno));
		throw std::runtime_error(error);
	}
	int pLen = int(int(packet[0])
		| int(packet[1]) << 8
		| int(packet[2]) << 16);
	// write(1, &packet[4], 1);
	write(fd, "****\n", 5);
	write(fd, date, strlen(date));
	write(fd, "\n", 1);
	write(fd, &packet[5], pLen - 1);
	write(fd, "\n****\n", 5);
	close (fd);
}