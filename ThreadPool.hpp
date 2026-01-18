#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <exception>

using namespace std;

/**
 * @class ThreadPool
 * @brief Simple thread pool implementation for executing tasks concurrently.
 *
 * This class manages a fixed number of worker threads that continuously
 * execute tasks stored in a shared queue. Tasks are submitted as callable
 * objects with no return value.
 */
class ThreadPool {

private:

    /**
     * @brief Pointer to the array of worker threads.
     */
    thread* th;

    /**
     * @brief Mutex used to protect access to the task queue.
     */
    mutex mtx;

    /**
     * @brief Queue that stores the tasks to be executed by the threads.
     */
    queue<function<void()>> q;

    /**
     * @brief Number of worker threads in the pool.
     */
    unsigned int num_threads;

    /**
     * @brief Flag indicating when threads should stop execution.
     */
    bool exit_flag;

    /**
     * @brief Indicates whether the thread pool is running correctly.
     */
    bool state;

    /**
     * @brief Auxiliary counter (currently unused).
     */
    int cont = 0;

public:

    /**
     * @brief Default constructor deleted to force explicit thread count.
     */
    ThreadPool() = delete;

    /**
     * @brief Constructs a ThreadPool with a fixed number of threads.
     * @param num_threads Number of worker threads to create.
     */
    ThreadPool(unsigned int num_threads);

    /**
     * @brief Submits a task to the thread pool.
     *
     * The task is stored in the internal queue and will be executed
     * by one of the worker threads.
     *
     * @tparam Func Callable type.
     * @tparam Args Argument types for the callable.
     * @param f Function to execute.
     * @param args Arguments passed to the function.
     */
    template <class Func, class... Args>
    void submit(Func&& f, Args&&... args);

    /**
     * @brief Stops the thread pool and optionally joins all threads.
     *
     * @param secure If true, waits for all threads to finish execution.
     */
    void finish(bool secure = true);

    /**
     * @brief Returns the current state of the thread pool.
     * @return True if the pool is active, false otherwise.
     */
    bool status();

    /**
     * @brief Busy-waits until the task queue becomes empty.
     */
    void wait();

    /**
     * @brief Destructor. Stops the pool and releases resources.
     */
    ~ThreadPool();
};

/**
 * @brief Adds a new task to the task queue.
 *
 * This method locks the queue mutex, pushes a new task,
 * and then unlocks the mutex.
 */
template <class Func, class... Args>
void ThreadPool::submit(Func&& f, Args&&... args) {

    mtx.lock();

    this->q.push(
        [&]() {
            f(args...);
        }
    );

    mtx.unlock();
}

#endif
