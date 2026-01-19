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
    function<void(unsigned int)> f = [&](unsigned int process_number) {

        function<void()> task;

        unique_lock<mutex> lock(this->mtx);

        while (!this->exit_flag.load() && this->state.load()) {


            cv.wait(lock,[&](){
                    return !this->q.empty() || (this->exit_flag.load() || !this->state.load());
                }
            );



            if(this->exit_flag.load() || !this->state.load()){
                lock.unlock();
                break;
            }

            task = move(this->q.front());
            this->q.pop();

            lock.unlock();


            this->process_flag[process_number]=true;
            try{
                task();
            }catch(...){
                this->error=true;
            }
            this->process_flag[process_number]=false;
            this->cv2.notify_one();
            lock.lock();
        }
    };

    this->exit_flag = false;

    this->th = new (nothrow) thread[num_threads];
    this->process_flag = new (nothrow) atomic<bool>[num_threads];

    if (this->th != nullptr && this->process_flag != nullptr) {
        this->state = true;

        try {
            for (unsigned int i = 0; i < this->num_threads; i++) {
                this->th[i] = thread(f,i);
                this->process_flag[i]=false;
            }
        } catch (exception&) {
            this->state = false;
            // cout << "The system is unable to start a new thread" << endl;
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
 * @brief Provides the number of process in queue.
 *
 * @return integer with the length of the queue.
 */
unsigned int ThreadPool::length() {
    return this->q.size();
}



/**
 * @brief Stops the thread pool and optionally joins threads.
 *
 * @param secure If true, waits for all threads to finish execution.
 */
void ThreadPool::finish(bool secure) {
    this->exit_flag = true;

    this->cv.notify_all();

    if (secure && this->state.load()) {
        for (unsigned int i = 0; i < this->num_threads; i++) {
            if (this->th[i].joinable()) {
                try {
                    this->th[i].join();
                } catch (...) {
                    // cout << "Error joining thread " << i << endl;
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
        delete[] this->process_flag;
    }catch(...){

    }
}

/**
 * @brief Waits until all queued tasks have been processed.
 */
void ThreadPool::wait() {
    unique_lock<mutex> lock(this->mtx);

    this->cv2.wait(lock,[&](){

            bool flag=true;

            if(this->q.empty()){

                for(unsigned int i=0;i<this->num_threads;i++){
                    if(this->process_flag[i].load()){
                        flag=false;
                        break;
                    }
                }

                return flag?true:false;
            }else{
                return false;
            }
        }
    );

}
