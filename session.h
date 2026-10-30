#ifndef SESSION_H
#define SESSION_H

#include "fd_wrapper.h"
#include <cstdint>
#include <sys/types.h>
#include <vector>

class Session {
public:
  enum State {
    CONNECTING_UPSTREAM, 
    ESTABLISHED,
    CLOSED  
  };

private:
  static constexpr ssize_t s_temp_buff_size{2048};

  FdWrapper m_client_fd;
  FdWrapper m_upstream_fd;

  std::vector<uint8_t> m_client_to_upstream_buffer;
  std::vector<uint8_t> m_upstream_to_client_buffer;

public:
  explicit Session(FdWrapper client_fd) noexcept;
  ssize_t read_from_client();
  State state() const noexcept { return m_state; }
  int client_fd() const noexcept { return m_client_fd.get(); }
  size_t client_buffer_size() const noexcept {
    return m_client_to_upstream_buffer.size();
  }
  State m_state{CONNECTING_UPSTREAM};
  bool connect_upstream(const char *ip, const char *port);
  ssize_t write_to_upstream();
  int upstream_fd() const noexcept { return m_upstream_fd.get(); }
};

#endif // !SESSION_H
