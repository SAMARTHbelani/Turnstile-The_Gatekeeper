# Turnstile

**Turnstile is an experimental, event-driven TCP forwarding proxy
written in C++20.**

The project is a hands-on exploration of POSIX socket programming,
non-blocking I/O, connection management, and resource ownership in C++.
It currently accepts TCP clients on a local listening socket and
forwards client data to a configured upstream endpoint.

> **Project status:** Work in progress. The current implementation is an
> early prototype, not a production-ready proxy. Bidirectional
> forwarding, robust asynchronous connection handling, bounded
> buffering, and comprehensive error handling are still being developed.

## Current scope

The current prototype includes:

-   A TCP listening socket created with POSIX networking APIs.
-   A `poll()`-based event loop for monitoring the listener and client
    connections.
-   Non-blocking socket setup using `fcntl()` and `O_NONBLOCK`.
-   A session abstraction that owns client and upstream socket
    descriptors.
-   An `FdWrapper` RAII type with deleted copy operations and move
    support.
-   A client-to-upstream data path using `recv()` and `send()`.
-   Basic handling of `EAGAIN` and `EWOULDBLOCK` while reading and
    writing.

The implementation is intentionally being developed incrementally. The
items above describe the current code structure; they should not be
interpreted as a claim that every socket lifecycle and failure case is
already handled correctly.

## Architecture

At a high level, the current code is organized into these components:

  -----------------------------------------------------------------------
  Component                           Responsibility
  ----------------------------------- -----------------------------------
  `Listener`                          Creates and owns the TCP listening
                                      socket and accepts clients.

  `EventLoop`                         Uses `poll()` to monitor socket
                                      readiness and dispatch events.

  `Session`                           Holds the client/upstream
                                      descriptors and the
                                      client-to-upstream buffer.

  `FdWrapper`                         Provides basic RAII ownership and
                                      move semantics for a file
                                      descriptor.

  `common`                            Contains shared networking
                                      utilities.
  -----------------------------------------------------------------------

The event loop is single-threaded. Rather than creating one thread per
client, it waits for readiness notifications from `poll()` and processes
the descriptors that become ready.

## Current network configuration

The prototype currently uses fixed endpoints in the source code:

  Setting                 Value
  ----------------------- -------------
  Client listening port   `8008`
  Upstream address        `127.0.0.1`
  Upstream port           `9090`
  Address family          IPv4
  Event notification      `poll()`

These values are not configurable through command-line arguments or a
configuration file yet.

## Build

### Requirements

-   Linux or macOS with POSIX socket APIs
-   A C++20-compatible compiler
-   GNU Make

The supplied Makefile defaults to `g++-16`. If that compiler is not
installed, override `CXX` with an available C++20 compiler.

``` bash
git clone https://github.com/SAMARTHbelani/turnstile.git
cd turnstile

# Build with the Makefile's default compiler (g++-16)
make

# Or select another compiler, for example:
make clean
make CXX=clang++
```

The current Makefile produces an executable named `gatekeeper`. The
executable name will be aligned with the project name as the
implementation is cleaned up.

To remove generated object files and the executable:

``` bash
make clean
```

## Running the prototype

The current source uses `127.0.0.1:9090` as its upstream and listens for
clients on port `8008`.

Open one terminal and start a local TCP service on port `9090`:

``` bash
nc -l 9090
```

In another terminal, run the proxy:

``` bash
./gatekeeper
```

In a third terminal, connect a client:

``` bash
nc 127.0.0.1 8008
```

This is an initial local smoke-test setup, not a complete
integration-test suite. Netcat command-line options vary between
operating systems and implementations.

## Engineering work in progress

The next development milestones are:

1.  Correctly complete non-blocking upstream connections, including
    `EINPROGRESS` and `SO_ERROR` handling.
2.  Implement forwarding in both directions, including partial writes
    and independent read/write readiness.
3.  Add bounded buffers and backpressure so a slow upstream cannot cause
    unbounded memory growth.
4.  Handle TCP EOF and half-close behavior without prematurely
    terminating the reverse direction.
5.  Improve handling of `EINTR`, socket errors, hang-ups, and
    interrupted or failed system calls.
6.  Make descriptor ownership consistent across all socket-owning
    classes.
7.  Add automated tests, sanitizer builds, and reproducible performance
    measurements.

## Design goals

The project is being developed around a few practical
systems-programming concerns:

-   **Explicit resource ownership:** file descriptors should have clear
    owners and deterministic cleanup.
-   **Non-blocking I/O:** socket operations should not stall the event
    loop while waiting for network activity.
-   **Correct stream handling:** TCP is a byte stream; reads and writes
    may process fewer bytes than requested.
-   **Bounded resource usage:** connection and buffer growth should be
    controlled.
-   **Observable behavior:** failures and connection state transitions
    should be diagnosable.

## License

No license has been added to the repository yet. Add a license file
before redistributing the project or accepting external contributions.
