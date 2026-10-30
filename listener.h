#ifndef LISTENER_H
#define LISTENER_H

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

#define PORT "8008" 

class Listener {
  static inline addrinfo s_hints{
    .ai_flags = AI_PASSIVE,
    .ai_family = PF_INET,
    .ai_socktype = SOCK_STREAM
  }; 
  int m_listener{-1};
  sockaddr_storage m_address{};

public:
  Listener();
  ~Listener();

  Listener(const Listener&) = delete;
  Listener& operator=(const Listener&) = delete;

  Listener(Listener&&);
  Listener& operator=(Listener&&);

  int start_tcp_server();

  void close_server();

  int accpet_new_connection();
  int fd() const;
};

#endif 
