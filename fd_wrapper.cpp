#include "fd_wrapper.h"
#include <unistd.h>

FdWrapper::~FdWrapper() {
  reset();
}

FdWrapper::FdWrapper(FdWrapper&& other) 
  : m_fd(other.m_fd) {
    other.m_fd = -1;
  }
    
FdWrapper &FdWrapper::operator=(FdWrapper&& other) {
  if (this != &other) {
    reset();
    m_fd = other.m_fd;
    other.m_fd = -1;
  }
  return *this;
}

int FdWrapper::get() const { return m_fd; }
bool FdWrapper::is_vaild() const { return m_fd >= 0; }

void FdWrapper::reset() {
  if (m_fd >= 0) {
    ::close(m_fd);
    m_fd = -1;
  }
}
