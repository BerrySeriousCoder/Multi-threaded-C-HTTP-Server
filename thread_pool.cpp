#include "thread_pool.h"
#include "http_server.h"

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { this->worker_thread(); });
    }
}

void ThreadPool::enqueue(int client_socket) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(client_socket);
    }
    condition.notify_one();
}

void ThreadPool::shutdown() {
    stop = true;
    condition.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        int client_socket;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
                return;
            client_socket = tasks.front();
            tasks.pop();
        }
        handle_client(client_socket);
    }
}
