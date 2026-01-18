#include "../include/test.hpp"



using namespace std;

void test1(){
    cout<<this_thread::get_id()<<" Proceso dentro de test1"<<endl;
}


void test2(int x,int y){
    cout<<this_thread::get_id()<<" Proceso dentro de test2 con suma "<<x+y<<endl;
}







void test4(){
    cout<<this_thread::get_id()<<" Proceso dentro de test4 sin argumentos"<<endl;
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
