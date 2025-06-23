#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    void enqueue(int client_socket);
    void shutdown();
private:
    std::vector<std::thread> workers;
    std::queue<int> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void worker_thread();
};

#endif
