#!/bin/bash


if [ -e example ]
then
    rm example
fi

if [ -e gmon.out ]
then
    rm gmon.out
fi

if [ -e stderr_test_thread.txt ]
then
    rm stderr_test_thread.txt
fi

if [ -e stdout_test_thread.txt ]
then
    rm stdout_test_thread.txt
fi

if [ -e stderr_test_memory.txt ]
then
    rm stderr_test_memory.txt
fi

if [ -e stdout_test_memory.txt ]
then
    rm stdout_test_memory.txt
fi

if [ -e lib/ThreadPool.so ]
then
    rm lib/ThreadPool.so
fi

if [ -e lib/ThreadPool.a ]
then
    rm lib/ThreadPool.a
fi

if [ -e build/ThreadPool.o ]
then
    rm build/ThreadPool.o
fi

if [ -e build/tests/test ]
then
    rm build/tests/test
fi

if [ -e build/tests/test_memory ]
then
    rm build/tests/test_memory
fi

if [ -e build/tests/test_resources ]
then
    rm build/tests/test_resources
fi

if [ -e build/tests/test_threads ]
then
    rm build/tests/test_threads
fi
