#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <thread>
#include <chrono>
#include <syncstream>

using namespace std;

void test1(int);
void test2(int,int);

template <class T>
void test3(T x,T y){
    cout<<this_thread::get_id()<<" Proceso dentro de test3 con suma "<<x+y<<endl;
}

template <class... Args>
void test4(int x,Args... args){
    cout<<this_thread::get_id()<<" Proceso dentro de test4 con argumento "<<x<<endl;
}

void test4();


void function1(int x,int y,int z);

template <class... Args>
void function2(void (*funcion)(int,int,int),Args... args);


#endif
