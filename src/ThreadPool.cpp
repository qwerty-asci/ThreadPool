/**
 * @file ThreadPool.cpp
 * @brief Implementation of the ThreadPool class.
 */

#include "../include/ThreadPool.hpp"

/**
 * @brief Constructs and starts the worker threads.
 *
 * Each worker thread runs a loop that retrieves tasks
 * from the queue and executes them until termination
 * is requested.
 */
ThreadPool::ThreadPool(unsigned int num_threads)
    : num_threads(num_threads) {

    /**
     * @brief Worker thread function.
     *
     * Continuously checks the task queue and executes tasks
     * while the pool is active and the exit flag is not set.
     */
    function<void(void)> f = [&]() {

        function<void()> task;

        unique_lock<mutex> lock(this->mtx);

        while (!this->exit_flag && this->state) {
            cv.wait(lock,[&](){return !this->q.empty();});

            if (!this->q.empty()) {
                task = move(this->q.front());
                this->q.pop();

                this->mtx.unlock();

                task();
            }
        }
    };

    exit_flag = false;

    this->th = new (nothrow) thread[num_threads];

    if (this->th != nullptr) {
        this->state = true;

        try {
            for (unsigned int i = 0; i < this->num_threads; i++) {
                this->th[i] = thread(f);
            }
        } catch (exception&) {
            this->state = false;
            cout << "The system is unable to start a new thread" << endl;
        }
    } else {
        this->state = false;
    }
}

/**
 * @brief Checks whether the thread pool is running.
 *
 * @return True if the pool is active, false otherwise.
 */
bool ThreadPool::status() {
    return this->state;
}

/**
 * @brief Stops the thread pool and optionally joins threads.
 *
 * @param secure If true, waits for all threads to finish execution.
 */
void ThreadPool::finish(bool secure) {
    this->exit_flag = true;

    if (secure && this->state) {
        for (unsigned int i = 0; i < this->num_threads; i++) {
            if (this->th[i].joinable()) {
                try {
                    this->th[i].join();
                } catch (...) {
                    cout << "Error joining thread " << i << endl;
                }
            }
        }
        this->state = false;
    }
}

/**
 * @brief Destructor.
 *
 * Ensures that the pool is stopped and memory is released.
 */
ThreadPool::~ThreadPool() {
    this->finish();
    try{
        delete[] this->th;
    }catch(...){

    }
}

/**
 * @brief Waits until all queued tasks have been processed.
 *
 * @warning This function uses busy waiting and may waste CPU time.
 */
void ThreadPool::wait() {
    while (!this->q.empty()) {
        // Busy wait
    }
}
