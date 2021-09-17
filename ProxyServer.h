/***************************************/
/*  By Mezin Georgy  created: 16.09.21 */
/***************************************/

#ifndef PROXYSERVER_HPP
#define PROXYSERVER_HPP

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#define BUFFSIZE 100000
#define COM_QUERY 3
#define COM_STM_PREPARE 22

void logger(char *packet);

#endif