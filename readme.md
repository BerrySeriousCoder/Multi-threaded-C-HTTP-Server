# Multi-threaded HTTP Server (C++20)

🚀 A lightweight, high-performance HTTP server written in **pure C++20** using **raw POSIX sockets** and a custom **thread pool** — no external libraries!  

Supports static file serving, basic REST APIs, and multi-client concurrency.

---

## 🌟 Features

✅ **Multi-threaded architecture** — handles multiple clients simultaneously with a custom thread pool  
✅ **Serve static files** — HTML, CSS, JS, etc. from a `www/` directory  
✅ **REST API endpoints**  
&nbsp;&nbsp;&nbsp;• `/api/hello` → `{"message": "Hello from REST API!"}`  
&nbsp;&nbsp;&nbsp;• `/api/time` → `{"time": "<current server time>"}`  
✅ **Security** — protects against directory traversal (e.g. `/../`)  
✅ **Content-Type detection** — responds with correct MIME type  
✅ **Graceful shutdown** — handles `Ctrl+C` cleanly  
✅ **C++20** — modern C++ code, no external dependencies  

---

## 🏗 Project Structure

/server
├── main.cpp # Server entry point
├── http_server.cpp # Request handling logic
├── http_server.h
├── thread_pool.cpp # Simple thread pool
├── thread_pool.h
└── www/ # Static files
├── index.html
└── style.css


---

## ⚡ How to Build

1️⃣ Clone the repo  
```bash
git clone https://github.com/yourusername/cpp-multithreaded-http-server.git
cd cpp-multithreaded-http-server

2️⃣ Build

bash

g++ -std=gnu++20 main.cpp http_server.cpp thread_pool.cpp -pthread -o server


How to Run

./server

By default, server listens on port 8080.

Then open:

http://localhost:8080/ → Serves index.html

http://localhost:8080/api/hello → JSON response

http://localhost:8080/api/time → Current server time in JSON

Or use curl:
curl http://localhost:8080/
curl http://localhost:8080/api/hello
curl http://localhost:8080/api/time


🛡 Example Security
✅ Directory traversal blocked:

bash

curl http://localhost:8080/../secret
# Returns 403 Forbidden or 404 Not Found

 How it Works
Socket server binds to port 8080 and listens for connections.

Each connection is handed off to a thread pool worker.

Parses HTTP requests (GET), serves files or REST API response.

Protects against malicious URL paths.

Logs each incoming request to console.

