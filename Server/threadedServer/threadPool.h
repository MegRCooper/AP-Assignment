// Code is used from: https://github.com/progschj with additional support from an example provided by the module coords.

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class ThreadPool {
public:
	ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future <typename std::result_of <F(Args...)>::type>;
    ~ThreadPool();
private:
    // Need to keep track of the threads so that they can be joined.
    std::vector < std::thread > workers;
    // Tasks queue:
    std::queue < std::function <void() >> tasks;

    // Syncronisation:
    std::mutex queueMutex;
    std::condition_variable condVar;
    bool stop;
};

// Constructor - launches some of the workers.
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; i++)
        workers.emplace_back(
            [this] {
                for (;;) {
                    std::function <void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condVar.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                            });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            }
    );
}

// Add new work items to the Pool.
template < class F, class ... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using returnType = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared <std::packaged_task <returnType()> >(
        std::bind(std::forward <F>(f), std::forward<Args>(args)...)
        );
    std::future <returnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        // Doesn't allow enqueueing after stopping the thread pool
        if (stop)
            throw std::runtime_error("Enqueue on stopped Thread Pool");
        tasks.emplace([task]() { (*task)(); });
    }
    condVar.notify_one();
    return (res);
}

// Destructor joins all threads.
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock <std::mutex> lock(queueMutex);
        stop = true;
    }
    condVar.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}
#endif