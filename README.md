> I know this readme is written using ai.
> Yes, I use AI to write my README files. Bite me. 🧛‍♂️

# Turnstile: C++20 Unidirectional TCP Proxy

Turnstile is a lightweight, high-performance TCP ingestion proxy built entirely from scratch in C++20. Acting as a one-way middleman (a data diode or telemetry forwarder), it multiplexes concurrent client connections and securely routes data to an upstream target without relying on heavy external networking frameworks.

## 🧠 Architecture & Technical Details

This project was built to demonstrate a deep understanding of POSIX systems programming, memory safety, and event-driven architectures:

* **Single-Threaded Event Loop:** Uses the POSIX `poll()` system call to monitor multiple socket descriptors concurrently, avoiding the massive memory overhead and context-switching of a thread-per-connection model.
* **Non-Blocking I/O State Machine:** All sockets are configured with `O_NONBLOCK`. The proxy handles partial reads/writes, buffers chunks in userspace vectors, and gracefully manages `EAGAIN`/`EWOULDBLOCK` signals without hanging the main thread.
* **Zero-Leak Resource Management:** Implements strict C++ RAII (Resource Acquisition Is Initialization) and move semantics to wrap volatile POSIX file descriptors. Sockets are guaranteed to be closed deterministically the moment a session goes out of scope or a connection drops.

## 🛠️ Build Instructions

**Prerequisites:**
* A Unix-like environment (macOS or Linux)
* A modern C++ compiler (GCC 10+ or Clang 10+ for C++20 support)
* CMake 3.15+

```bash
# Clone the repository
git clone https://github.com/yourusername/turnstile-proxy.git
cd turnstile-proxy

# Generate build files and compile
mkdir build && cd build
cmake ..
make
```

## 🚀 Usage & End-to-End Testing

Because Turnstile is a Layer 4 TCP proxy, you can test it easily using the standard Unix `nc` (Netcat) utility. You will need three separate terminal tabs.

**Terminal 1: Start the Upstream Server**
This acts as your secure backend database or log aggregator.
```bash
nc -l 9090
```

**Terminal 2: Start Turnstile**
```bash
./gatekeeper
```

**Terminal 3: Connect the Client**
Connect to the proxy and start typing.
```bash
nc localhost 8008
```

Type a message into Terminal 3 and press Enter. You will see Turnstile act as the perfect middleman, buffering the bytes and immediately routing them out to Terminal 1!

## 📜 License
MIT License