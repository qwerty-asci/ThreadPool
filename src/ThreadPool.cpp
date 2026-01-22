/**
 * @file ThreadPool.cpp
 * @brief Implementation of the ThreadPool class.
 */

#include "../include/ThreadPool.hpp"

/**
 * @brief Constructs the thread pool and starts the worker threads.
 *
 * Each worker thread runs a loop that retrieves tasks
 * from the queue and executes them until termination
 * is requested.
 *
 * @param num_threads Number of worker threads to create.
 */
ThreadPool::ThreadPool(unsigned int num_threads)
    : num_threads(num_threads) {

    /**
     * @brief Worker thread routine.
     *
     * Each worker thread continuously waits for tasks
     * in the queue and executes them while the pool
     * remains active and no exit has been requested.
     *
     * @param process_number Index of the worker thread.
     */
    function<void(unsigned int)> f = [&](unsigned int process_number) {

        function<void()> task;

        unique_lock<mutex> lock(this->mtx);

        while (!this->exit_flag.load() && this->state.load()) {

            cv.wait(lock, [&]() {
                return !this->q.empty()
                       || this->exit_flag.load()
                       || !this->state.load();
            });

            if (this->exit_flag.load() || !this->state.load()) {
                lock.unlock();
                break;
            }

            task = move(this->q.front());
            this->q.pop();

            lock.unlock();

            this->process_flag[process_number] = true;
            try {
                task();
            } catch (...) {
                this->error = true;
            }
            this->process_flag[process_number] = false;

            lock.lock();
            if (this->waiting.load()) {
                this->cv2.notify_one();
            }
        }

        if (lock.owns_lock()) {
            lock.unlock();
        }
    };

    this->exit_flag = false;
    this->waiting = false;

    this->th = new (nothrow) thread[num_threads];
    this->process_flag = new (nothrow) atomic<bool>[num_threads];

    if (this->th != nullptr && this->process_flag != nullptr) {
        this->state = true;

        try {
            for (unsigned int i = 0; i < this->num_threads; i++) {
                this->th[i] = thread(f, i);
                this->process_flag[i] = false;
            }
        } catch (exception&) {
            this->state = false;
            // Failed to start one or more worker threads
        }
    } else {
        this->state = false;
    }
}

/**
 * @brief Checks whether the thread pool is currently running.
 *
 * @return True if the pool is active, false otherwise.
 */
bool ThreadPool::status() {
    return this->state;
}

/**
 * @brief Returns the number of pending tasks in the queue.
 *
 * @return Number of tasks currently stored in the queue.
 */
unsigned int ThreadPool::length() {
    return this->q.size();
}

/**
 * @brief Stops the thread pool and optionally joins worker threads.
 *
 * Signals all threads to terminate and wakes them if they are waiting.
 * If secure is enabled, this function blocks until all worker threads
 * have finished execution.
 *
 * @param secure If true, waits for all threads to finish execution.
 */
void ThreadPool::finish(bool secure) {
    this->exit_flag = true;

    unique_lock<mutex> lock(this->mtx);
    this->cv.notify_all();
    lock.unlock();

    if (secure && this->state.load()) {
        for (unsigned int i = 0; i < this->num_threads; i++) {
            if (this->th[i].joinable()) {
                try {
                    this->th[i].join();
                } catch (...) {
                    // Error while joining a worker thread
                }
            }
        }
        this->state = false;
    }
}

/**
 * @brief Destructor.
 *
 * Ensures that the thread pool is stopped and all
 * allocated resources are released.
 */
ThreadPool::~ThreadPool() {
    this->finish();
    try {
        delete[] this->th;
        delete[] this->process_flag;
    } catch (...) {
        // Suppress all exceptions during cleanup
    }
}

/**
 * @brief Waits until all queued tasks have been processed.
 *
 * Blocks the calling thread until the task queue is empty
 * and no worker thread is currently executing a task.
 */
void ThreadPool::wait() {
    unique_lock<mutex> lock(this->mtx);

    this->waiting = true;

    this->cv2.wait(lock, [&]() {

        bool finished = true;

        if (this->q.empty()) {
            for (unsigned int i = 0; i < this->num_threads; i++) {
                if (this->process_flag[i].load()) {
                    finished = false;
                    break;
                }
            }
            return finished;
        }
        return false;
    });

    this->waiting = false;
}
