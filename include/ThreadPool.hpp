
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

    /** Mutex protecting access to the task queue */
    mutex mtx;

    /** Queue containing pending tasks */
    queue<function<void()>> q;

    /** Number of threads managed by the pool */
    unsigned int num_threads;

    /** Flag indicating when threads should terminate */
    atomic<bool> exit_flag;

    /** Array of flags indicating if a thread is running a process or not */
    atomic<bool> * process_flag;

    /** Indicates whether the thread pool is active */
    atomic<bool> state;

    /** Condition variable used to stop the threads until a new process is pushed */
    condition_variable cv;

    /** Condition variable used to wait until all the queue finish */
    condition_variable cv2;

public:

    /**
     * @brief Deleted default constructor.
     *
     * Forces the user to specify the number of threads.
     */
    ThreadPool() = delete;

    /**
     * @brief Constructs a ThreadPool with a fixed number of threads.
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
     * @tparam Func Callable object type.
     * @tparam Args Argument types for the callable.
     * @param f Function to execute.
     * @param args Arguments passed to the function.
     *
     */
    template <class Func, class... Args>
    void submit(Func&& f, Args&&... args);

    /**
     * @brief Stops the thread pool.
     *
     * Sets the exit flag and optionally waits for all threads to finish.
     *
     * @param secure If true, joins all worker threads.
     */
    void finish(bool secure = true);

    /**
     * @brief Returns the current state of the thread pool.
     *
     * @return True if the pool is running, false otherwise.
     */
    bool status();


    /**
     * @brief Returns the number of process in queue.
     *
     * @return integer with the length of the queue.
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
     * Stops the pool and releases all allocated resources.
     */
    ~ThreadPool();


    /**
     * @param Flag that idicates if any thread has thrown an exception
     *
     */
    atomic<bool> error=false;
};

/**
 * @brief Adds a new task to the task queue.
 *
 * Locks the mutex, pushes the task into the queue,
 * and then unlocks the mutex.
 */
template <class Func, class... Args>
void ThreadPool::submit(Func&& f, Args&&... args) {


    {
        lock_guard<mutex> lock(this->mtx);


        this->q.push(
            move(
                [&]() {
                    f(args...);
                }
            )
        );
    }

    this->cv.notify_one();
}

#endif
