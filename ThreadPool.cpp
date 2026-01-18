#include "ThreadPool.hpp"

/**
 * @brief Creates and starts the worker threads.
 *
 * Each worker thread continuously checks the task queue and
 * executes available tasks until the exit flag is set.
 */
ThreadPool::ThreadPool(unsigned int num_threads)
    : num_threads(num_threads) {

    /**
     * @brief Worker thread function.
     *
     * Threads repeatedly fetch tasks from the queue and execute them
     * while the pool is active and the exit flag is not set.
     */
    function<void(void)> f = [&]() {

        function<void()> f;

        while (!this->exit_flag && this->state) {
            mtx.lock();

            if (!this->q.empty()) {
                f = this->q.front();
                q.pop();

                mtx.unlock();

                f();
            } else {
                mtx.unlock();
            }
        }
    };

    exit_flag = false;

    // Allocate thread array
    this->th = new (nothrow) thread[num_threads];

    if (this->th != nullptr) {
        this->state = true;

        try {
            for (unsigned int i = 0; i < this->num_threads; i++) {
                this->th[i] = thread(f);
            }
        } catch (exception& e) {
            this->state = false;
            cout << "The system is unable to start a new thread" << endl;
        }

    } else {
        this->state = false;
    }
}

/**
 * @brief Returns whether the thread pool is running.
 */
bool ThreadPool::status() {
    return this->state;
}

/**
 * @brief Signals all threads to stop and optionally joins them.
 *
 * If secure mode is enabled, this function waits for all
 * worker threads to terminate before returning.
 */
void ThreadPool::finish(bool secure) {
    this->exit_flag = true;

    if (secure && this->state) {
        for (unsigned int i = 0; i < this->num_threads; i++) {
            if (this->th[i].joinable()) {
                try {
                    this->th[i].join();
                } catch (...) {
                    cout << "Error joining the thread i" << endl;
                }
            }
        }
        this->state = false;
    }
}

/**
 * @brief Destructor.
 *
 * Ensures that all threads are stopped and memory is released.
 */
ThreadPool::~ThreadPool() {

    this->finish();
    delete[] this->th;
}

/**
 * @brief Waits until the task queue becomes empty.
 *
 * This method performs a busy wait and does not block efficiently.
 */
void ThreadPool::wait() {
    while (!this->q.empty()) {
        // Busy wait
    }
}
