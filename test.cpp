#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>


using namespace std;

void test1(){
    cout<<"None"<<endl;
}


template <class T,class... T2>
void test1(T size,T2... args){

    cout<<size<<endl;

    test1(args...);
}


void function1(int x,int y,int z){
    cout<<x+y+x<<endl;

}

template <class... Args>
void function2(void (*funcion)(int,int,int),Args... args){

    cout<<"function2 "<<endl;
    funcion(2,3,4);
    funcion(args...);
}


void test2(int&& x){
    cout<<x<<endl;
}



struct class_test{
public:
    class_test()=default;

    void test(){
        cout<<"Esto es un test"<<endl;
    }
};

int main(){

    int x=100;


    // test1(5,1,2,34,5,2,3,"asd",3114,'a',true);
/*
    thread t(function2<int,int,int>,function1,1,2,3);
    //
    t.join();

    thread t2;

    t2=thread(function2<int,int,int>,function1,1,2,3);

    t2.join();*/
    // function<void(int,int,int)> f=function1;


    // function2<int,int,int>(function1,2,332423,23);

    // test2(x);/*

    // class_test ct{};
    //
    // thread t2(&class_test::test,ct);
    //
    // t2.join();

    queue<int> q;
    int aux;

    q.push(1);
    q.push(2);
    q.push(3);

    aux=q.front();
    cout<<q.size()<<" "<<aux<<endl;
    q.pop();
    aux=q.front();
    cout<<q.size()<<" "<<aux<<endl;
    q.pop();
    aux=q.front();
    cout<<q.size()<<" "<<aux<<endl;
    q.pop();

    return 0;
}
