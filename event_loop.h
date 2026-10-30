#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "listener.h"
#include "session.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <poll.h>

class EventLoop {
public:
  EventLoop() = default;
  void run();
private:
  Listener m_listener;
  std::unordered_map<int, std::unique_ptr<Session>> m_sessions;
  std::vector<struct pollfd> m_pollfds;
  void rebuild_pollfds();
  void handle_listener();
  void handle_client_read(int client_fd);
  void cleanup_closed_sessions();
};

#endif
