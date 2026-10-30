#include "event_loop.h"

int main () {
  EventLoop loop{};
  loop.run();
  return 0;
}
