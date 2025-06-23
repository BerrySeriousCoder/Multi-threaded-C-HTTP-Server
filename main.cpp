#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic>
#include <csignal>
#include "thread_pool.h"
#include "http_server.h"

const int PORT = 8080;
std::atomic<bool> stop_flag(false);
int global_server_fd = -1; 

void signal_handler(int signal) {
    std::cout << "\n[INFO] Caught signal " << signal << ", shutting down..." << std::endl;
    stop_flag = true;
    if (global_server_fd != -1) {
        close(global_server_fd); 
    }
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    global_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int server_fd = global_server_fd; 
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        return 1;
    }

    std::cout << "[INFO] Server listening on port " << PORT << "...\n";

    ThreadPool pool(4);

    while (!stop_flag) {
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (stop_flag) break;
        if (client_socket >= 0) {
            pool.enqueue(client_socket);
        }
    }

    close(server_fd);
    pool.shutdown();
    std::cout << "[INFO] Server shutdown complete.\n";
    return 0;
}
