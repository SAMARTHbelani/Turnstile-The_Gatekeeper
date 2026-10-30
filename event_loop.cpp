#include "event_loop.h"
#include <fcntl.h>
#include <iostream>

void EventLoop::run() {
  std::cout << "Starting Gatekeeper Event Loop...\n";

  while (true) {
    rebuild_pollfds();
    int ready_count = poll(m_pollfds.data(), static_cast<nfds_t>(m_pollfds.size()), -1);
    if (ready_count < 0) {
      perror("poll failed");
      break;
    }
    for (const auto& pfd : m_pollfds) {
      if (pfd.revents & POLLOUT) {
          for (auto& [client_fd_key, session] : m_sessions) {
              if (session->upstream_fd() == pfd.fd) {
                  session->write_to_upstream();
                  break;
              }
          }
      }
      if (pfd.revents == 0) continue; 
      if (pfd.revents & POLLIN) {
        if (pfd.fd == m_listener.fd()) { 
          handle_listener();
        } 
        else if (m_sessions.count(pfd.fd)) {
          handle_client_read(pfd.fd);
        }
      }
      if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL)) {
        if (m_sessions.count(pfd.fd)) {
           m_sessions[pfd.fd]->m_state = Session::CLOSED; 
        }
      }
    }
    cleanup_closed_sessions();
  }
}

void EventLoop::rebuild_pollfds() {
  m_pollfds.clear();
  m_pollfds.push_back({m_listener.fd(), POLLIN, 0}); 
  for (const auto& [fd, session] : m_sessions) {
    if (session->state() != Session::CLOSED) {
      m_pollfds.push_back({fd, POLLIN, 0});
      if (session->client_buffer_size() > 0 && session->upstream_fd() != -1) {
          m_pollfds.push_back({session->upstream_fd(), POLLOUT, 0});
      }
    }
  }
}

void EventLoop::handle_listener() {
  FdWrapper new_client = FdWrapper(m_listener.accpet_new_connection());
  auto condition = new_client.is_vaild();
  if (condition) {
    int fd = new_client.get();
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    auto session = std::make_unique<Session>(std::move(new_client));
    if (session->connect_upstream("127.0.0.1", "9090")) {
        m_sessions[fd] = std::move(session);
    } else {
        std::cout << "Failed to connect upstream.\n";
    }
  }
}

void EventLoop::handle_client_read(int client_fd) {
  auto& session = m_sessions[client_fd];
  ssize_t bytes = session->read_from_client();
  if (bytes > 0) {
    std::cout << "Read " << bytes << " bytes from client " << client_fd 
              << ". Buffer now holds " << session->client_buffer_size() << " bytes.\n";
  } else if (bytes == 0) {
    std::cout << "Client " << client_fd << " disconnected gracefully.\n";
  } else {
    std::cout << "Error reading from client " << client_fd << ".\n";
  }
}

void EventLoop::cleanup_closed_sessions() {
  for (auto it = m_sessions.begin(); it != m_sessions.end(); ) {
    if (it->second->state() == Session::CLOSED) {
      std::cout << "Cleaning up closed session for FD " << it->first << "\n";
      it = m_sessions.erase(it);
    } else {
      ++it;
    }
  }
}
