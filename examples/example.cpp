#include <iostream>
#include "ThreadPool.hpp"
#include "test.hpp"


using namespace std;


int main(int argc, char * argv[]){


	ThreadPool thp{4};

	for(int i=0;i<20;i++){
		thp.submit(test1,i);
		// this_thread::sleep_for(20ms);
	}

	// cout<<"En proceso"<<endl;

	thp.wait();

	if(thp.status()){
		thp.finish();
	}

	if(!thp.status()){
		cout<<"Cerrado correctamente"<<endl;
	}

	return 0;
}
