#include "common.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
const char *inet_ntop2(struct sockaddr_storage *addr, char *buff,
                       socklen_t len) {
  struct in_addr *ipv4;
  struct in6_addr *ipv6;
  void *src;

  if (addr->ss_family == PF_INET) {
    ipv4 = &(((struct sockaddr_in *)addr)->sin_addr);
    src = ipv4;
  } else {
    ipv6 = &(((struct sockaddr_in6 *)addr)->sin6_addr);
    src = ipv6;
  }

  return inet_ntop(addr->ss_family, src, buff, len);
}
