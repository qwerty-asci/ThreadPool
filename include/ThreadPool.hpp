/**
 * @file ThreadPool.hpp
 * @author qwert-asci
 * @brief Declaration of a simple ThreadPool class.
 *
 * This file contains the declaration of a basic thread pool
 * that executes tasks using a fixed number of worker threads.
 */

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <exception>
#include <condition_variable>
#include <atomic>
#include <utility>

using namespace std;

/**
 * @class ThreadPool
 * @brief Manages a pool of worker threads to execute tasks concurrently.
 *
 * The ThreadPool maintains a queue of tasks. Worker threads continuously
 * fetch and execute tasks from this queue until the pool is stopped.
 *
 * @note Tasks have no return value.
 */
class ThreadPool {

private:

    /** Pointer to the array of worker threads */
    thread* th;

    /** Mutex that protects access to the task queue */
    mutex mtx;

    /** Queue that stores pending tasks */
    queue<function<void()>> q;

    /** Number of threads managed by the pool */
    unsigned int num_threads;

    /** Flag indicating when threads should terminate */
    atomic<bool> exit_flag;

    /** Array of flags indicating whether each thread is executing a task */
    atomic<bool>* process_flag;

    /** Indicates whether the thread pool is currently running */
    atomic<bool> state;

    /** Indicates whether the pool is waiting for all tasks to finish */
    atomic<bool> waiting;

    /** Condition variable used to block threads until a new task is available */
    condition_variable cv;

    /** Condition variable used to wait until all tasks in the queue are completed */
    condition_variable cv2;

public:

    /**
     * @brief Deleted default constructor.
     *
     * Forces the user to specify the number of threads.
     */
    ThreadPool() = delete;

    /**
     * @brief Constructs a ThreadPool with a fixed number of worker threads.
     *
     * @param num_threads Number of worker threads to create.
     */
    explicit ThreadPool(unsigned int num_threads);

    /**
     * @brief Submits a task to the thread pool.
     *
     * The task is stored in the internal queue and will be executed
     * by one of the worker threads.
     *
     * @tparam Func Type of the callable object.
     * @tparam Args Types of the arguments passed to the callable.
     * @param f Function to execute.
     * @param args Arguments passed to the function.
     */
    template <class Func, class... Args>
    void submit(Func&& f, Args&&... args);

    /**
     * @brief Stops the thread pool.
     *
     * Sets the exit flag and optionally waits for all worker threads to finish.
     *
     * @param secure If true, joins all worker threads before returning.
     */
    void finish(bool secure = true);

    /**
     * @brief Returns the current state of the thread pool.
     *
     * @return True if the pool is running, false otherwise.
     */
    bool status();

    /**
     * @brief Returns the number of pending tasks in the queue.
     *
     * @return Number of tasks currently in the queue.
     */
    unsigned int length();

    /**
     * @brief Waits until the task queue becomes empty.
     *
     * @note This function performs a busy wait.
     */
    void wait();

    /**
     * @brief Destructor.
     *
     * Stops the thread pool and releases all allocated resources.
     */
    ~ThreadPool();

    /**
     * @brief Indicates whether any worker thread has thrown an exception.
     */
    atomic<bool> error = false;
};

/**
 * @brief Adds a new task to the task queue.
 *
 * Locks the mutex, pushes the task into the queue,
 * and notifies one waiting worker thread.
 */
template <class Func, class... Args>
void ThreadPool::submit(Func&& f, Args&&... args) {

    lock_guard<mutex> lock(this->mtx);

    this->q.push(
        move(
            [F = forward<Func>(f), ...args2 = forward<Args>(args)]() mutable {
                F(args2...);
            }
        )
    );

    this->cv.notify_one();
}

#endif

