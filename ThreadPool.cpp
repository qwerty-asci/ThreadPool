#include "ThreadPool.hpp"


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



