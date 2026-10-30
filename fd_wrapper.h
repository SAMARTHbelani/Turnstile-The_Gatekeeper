#ifndef FDWRAPPER_H
#define FDWRAPPER_H

#include <unistd.h>

class FdWrapper {
  int m_fd{};
public:
  FdWrapper() : m_fd{-1} {}
  FdWrapper(int fd) : m_fd{fd} {}

  ~FdWrapper();

  FdWrapper(const FdWrapper&) = delete;
  FdWrapper& operator=(const FdWrapper&) = delete;

  FdWrapper(FdWrapper&&);
  FdWrapper &operator=(FdWrapper&&);

  int get() const;
  bool is_vaild() const;

  void reset();
};

#endif
