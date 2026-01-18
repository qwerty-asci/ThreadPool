#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <exception>


using namespace std;



class ThreadPool{

private:

	thread * th;
	mutex mtx;
	queue<function<void()>> q;
	unsigned int num_threads;
	bool exit_flag,state;
	int cont=0;



public:

	ThreadPool()=delete;
	ThreadPool(unsigned int num_threads);

	template <class Func,class... Args>
	void submit(Func&& f,Args&&... args);
	void finish(bool secure=true);
	bool status();
	void wait();
	~ThreadPool();


};





template <class Func,class... Args>
void ThreadPool::submit(Func&& f,Args&&... args){

	mtx.lock();

	this->q.push(
		[&](){
			f(args...);
		}
	);

	mtx.unlock();

}

#endif
