#include "listener.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"

Listener::Listener() : m_listener{start_tcp_server()} {
  if (m_listener != -1) {
    char s[INET6_ADDRSTRLEN];
    const char* ip = inet_ntop2(&m_address, s, sizeof s);
    std::cout << "Started The server at " << (ip ? ip : "UNKNOWN") << std::endl;
  } else {
    std::cerr << "CRITICAL: Listener failed to start. Port might be in use." << std::endl;
    exit(1); 
  }
}

Listener::~Listener() {
  close(m_listener);
}

Listener::Listener(Listener &&other) : m_listener{other.m_listener}, m_address(other.m_address) {
  other.m_listener = -1;
  memset(&other.m_address, 0, sizeof other.m_address);
}

Listener &Listener::operator=(Listener &&other) {
  if (this != &other) {
    m_listener = other.m_listener;
    m_address = other.m_address;
    other.m_listener = -1;
    memset(&other.m_address, 0, sizeof other.m_address);
  }
  return *this;
}

int Listener::start_tcp_server() {
  int listener_fd{};

  int get_addr_error{};
  addrinfo *got_address{};
  if ((get_addr_error = getaddrinfo(NULL, PORT, &s_hints, &got_address)) != 0) {
    std::cerr << "Server - getaddrinfo: " << gai_strerror(get_addr_error);
    assert(get_addr_error == 0);
  }

  addrinfo *p{};
  for (p = got_address; p != nullptr; p = p->ai_next) {
    if ((listener_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("Server - socket: ");
      continue;
    }
    int flag {fcntl(listener_fd, F_GETFL, 0)};
    fcntl(listener_fd, F_SETFL, flag | O_NONBLOCK);
    int yes = 1;
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if (bind(listener_fd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("Server - bind: ");
      close(listener_fd);
      continue;
    }
    break;
  }

  if (p == nullptr) {
    std::cerr << "Didn't worked got no address\n";
    close(listener_fd);
    return -1;
  }

  memcpy(&m_address, p->ai_addr, p->ai_addrlen);
  freeaddrinfo(got_address);

  if (listen(listener_fd, 10) == -1) {
    perror("Server - listen: ");
    return -1;
  }
  return listener_fd;
}

void Listener::close_server() {
  close(m_listener);
  m_listener = -1;
  memset(&m_address, 0, sizeof m_address);
}

int Listener::accpet_new_connection() {
  sockaddr_storage reviver_addr {};
  socklen_t reviver_addr_len {sizeof reviver_addr};
  char s[INET6_ADDRSTRLEN];
  int new_connection_fd{-1};
  if ((new_connection_fd = accept(m_listener, (sockaddr*) &reviver_addr, &reviver_addr_len)) == -1) {
    perror("accept");
  }

  if (new_connection_fd != -1) {
    std::cout << "New connection connected to " << inet_ntop2(&reviver_addr, s, reviver_addr_len) << " at fd " << new_connection_fd << '\n';
  }

  return new_connection_fd;
}

int Listener::fd() const{
  return m_listener;
}
