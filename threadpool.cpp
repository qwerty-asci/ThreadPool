#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <exception>
#include "test.hpp"


using namespace std;



class ThreadPool{

private:

	thread * th;
	mutex mtx;
	queue<function<void()>> q;
	unsigned int num_threads;
	bool exit_flag,state;
	int cont=0;



	//Customized exceptions

	// class exception_thread_allocation:public exception{
	// 	virtual const char* what() const throw(){
	// 		return "My exception happened";
	// 	}
	// };

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


ThreadPool::ThreadPool(unsigned int num_threads):num_threads(num_threads){



	function<void(void)> f=[&](){

					function<void()> f;

					while(!this->exit_flag && this->state){
						mtx.lock();

						if(!this->q.empty()){
							f=this->q.front();
							q.pop();

							mtx.unlock();

							f();
						}else{
							mtx.unlock();
						}
					}
				};







	exit_flag=false;

	this->th=new (nothrow) thread[num_threads];

	if(this->th!=nullptr){
		this->state=true;

		try{
			for(unsigned int i=0;i<this->num_threads;i++){
				this->th[i]=thread(f);
			}
		}catch(exception &e){
			this->state=false;

			cout<<"The system is unable to start a new thread"<<endl;

		}

	}else{
		this->state=false;
	}

}

bool ThreadPool::status(){
	return this->state;
}

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



void ThreadPool::finish(bool secure){
	this->exit_flag=true;

	if(secure && this->state){
		for(unsigned int i=0;i<this->num_threads;i++){
			if(this->th[i].joinable()){
				try{
					this->th[i].join();
				}catch(...){
					cout<<"Error joining the thread i"<<endl;
				}
			}
		}
		this->state=false;
	}
}


ThreadPool::~ThreadPool(){

	this->finish();

	delete[] this->th;
}



void ThreadPool::wait(){
	while(!this->q.empty()){

	}
}




int main(int argc, char * argv[]){


	ThreadPool thp{10};

	for(int i=0;i<20;i++){
		thp.submit(test1);
	}


	thp.wait();

	if(thp.status()){
		thp.finish();
	}

	if(!thp.status()){
		cout<<"Cerrado correctamente"<<endl;
	}

	return 0;
}
