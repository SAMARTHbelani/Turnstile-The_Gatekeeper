#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <cstddef>
#include <iostream>
#include <sys/socket.h>

const char *inet_ntop2(struct sockaddr_storage *addr, char *buff, socklen_t len);

#endif // !COMMON_H

