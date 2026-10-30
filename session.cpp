#include "session.h"
#include <netdb.h>
#include <fcntl.h>
#include <cstring>
#include "fd_wrapper.h"
#include <cerrno>
#include <sys/socket.h>
#include <sys/wait.h>

Session::Session(FdWrapper client_fd) noexcept
  : m_client_fd(std::move(client_fd))  {}

ssize_t Session::read_from_client() {
  char temp_buff[s_temp_buff_size];
  ssize_t total_bytes_read = 0; 

  while (true) {
    ssize_t bytes_read = recv(m_client_fd.get(), temp_buff, sizeof temp_buff, 0);

    if (bytes_read > 0) {
      m_client_to_upstream_buffer.insert(m_client_to_upstream_buffer.end(), temp_buff, temp_buff+bytes_read);
      total_bytes_read += bytes_read;
    }

    else if (bytes_read == 0) {
      m_state = CLOSED;
      return 0;
    }

    else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      else {
        m_state = CLOSED;
        return -1;
      }
    }
  }

  return total_bytes_read;
}


bool Session::connect_upstream(const char* ip, const char* port) {
    addrinfo hints{};
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* resolved{};
    if (getaddrinfo(ip, port, &hints, &resolved) != 0) {
        return false;
    }
    int u_fd = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol);
    if (u_fd == -1) {
        freeaddrinfo(resolved);
        return false;
    }
    int flags = fcntl(u_fd, F_GETFL, 0);
    fcntl(u_fd, F_SETFL, flags | O_NONBLOCK);
    connect(u_fd, resolved->ai_addr, resolved->ai_addrlen);
    freeaddrinfo(resolved);
    m_upstream_fd = FdWrapper{u_fd};
    m_state = ESTABLISHED;
    return true;
}

ssize_t Session::write_to_upstream() {
    if (m_client_to_upstream_buffer.empty() || !m_upstream_fd.is_vaild()) {
        return 0;
    }
    ssize_t bytes_sent = send(m_upstream_fd.get(), m_client_to_upstream_buffer.data(), m_client_to_upstream_buffer.size(), 0);

    if (bytes_sent > 0) {
        m_client_to_upstream_buffer.erase(
            m_client_to_upstream_buffer.begin(), 
            m_client_to_upstream_buffer.begin() + bytes_sent
        );
    } else if (bytes_sent == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        m_state = CLOSED;
    }

    return bytes_sent;
}
