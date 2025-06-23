#include "http_server.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>

static bool ends_with(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

static std::string get_content_type(const std::string& path) {
    if (ends_with(path, ".html")) return "text/html";
    if (ends_with(path, ".css")) return "text/css";
    if (ends_with(path, ".js")) return "application/javascript";
    if (ends_with(path, ".json")) return "application/json";
    return "text/plain";
}

static bool is_safe_path(const std::string& path) {
    return path.find("..") == std::string::npos;
}

void handle_client(int client_socket) {
    char buffer[4096] = {0};
    read(client_socket, buffer, 4096);

    std::string request(buffer);
    std::istringstream req_stream(request);
    std::string method, url, version;
    req_stream >> method >> url >> version;

    if (method.empty() || url.empty() || version.empty()) {
        std::string response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 12\r\n\r\n"
            "Bad Request";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        std::cout << "[WARN] Malformed request received." << std::endl;
        return;
    }

    std::cout << "[REQUEST] Method: " << method << ", URL: " << url << ", Version: " << version << std::endl;

    if (url.find("..") != std::string::npos || !is_safe_path(url)) {
        std::string response =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n\r\n"
            "403 Forbidden";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        std::cout << "[INFO] Returning 403 for " << url << std::endl;
        return;
    }

    if (url == "/") url = "/index.html";

    if (url.rfind("/api/hello", 0) == 0) {
        std::string body = R"({"message": "Hello from REST API!"})";
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: application/json\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "\r\n"
            << body;
        std::string response = oss.str();
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        std::cout << "[INFO] Handling REST API call: " << url << std::endl;
        return;
    }

    if (url.rfind("/api/time", 0) == 0) {
        time_t now = time(0);
        std::string t = ctime(&now);
        t.pop_back(); // remove trailing \n
        std::string body = "{\"time\": \"" + t + "\"}";
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: application/json\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "\r\n"
            << body;
        std::string response = oss.str();
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        std::cout << "[INFO] Handling REST API call: " << url << std::endl;
        return;
    }

    std::string filepath = "www" + url;
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
        std::ostringstream oss;
        oss << "HTTP/1.1 404 Not Found\r\n"
            << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "\r\n"
            << body;
        std::string response = oss.str();
        send(client_socket, response.c_str(), response.size(), 0);
        std::cout << "[INFO] Returning 404 for " << url << std::endl;
    } else {
        std::ostringstream file_data;
        file_data << file.rdbuf();
        std::string body = file_data.str();
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: " << get_content_type(filepath) << "\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "\r\n"
            << body;
        std::string response = oss.str();
        send(client_socket, response.c_str(), response.size(), 0);
        std::cout << "[INFO] Served file: " << filepath << std::endl;
    }

    close(client_socket);
}
