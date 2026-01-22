#include <iostream>
#include <atomic>
#include <cassert>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>

#include "../include/ThreadPool.hpp"

using namespace std;

/* ============================================================
   UTILIDADES
   ============================================================ */

#define TEST_OK(name) cout << "[OK] " << name << endl

/* ============================================================
   TESTS FUNCIONALES
   ============================================================ */

void test_basic_execution(ThreadPool& pool) {
    constexpr int N = 20;
    atomic<int> counter{0};


    for (int i = 0; i < N; ++i) {
        pool.submit([](atomic<int>& counter) {
            counter.fetch_add(1, memory_order_relaxed);
        },ref(counter));
    }



    pool.wait();

    assert(counter == N);
    TEST_OK("test_basic_execution");
}

void test_reuse_after_wait(ThreadPool& pool) {
    atomic<int> counter{0};

    for (int i = 0; i < 1000; ++i)
        pool.submit([](atomic<int>& counter) { ++counter; },ref(counter));

    pool.wait();

    for (int i = 0; i < 1000; ++i)
        pool.submit([](atomic<int>& counter) { ++counter; },ref(counter));

    pool.wait();

    assert(counter == 2000);
    TEST_OK("test_reuse_after_wait");
}

/* ============================================================
   CONCURRENCIA Y SEGURIDAD
   ============================================================ */

void test_multiple_producers(ThreadPool& pool) {
    constexpr int producers = 8;
    constexpr int tasks_per_producer = 2000;

    atomic<int> counter{0};
    thread * th;

    th=new thread[producers];

    for (int i = 0; i < producers; ++i) {
        th[i]=thread([](atomic<int>& counter,ThreadPool& pool) {
                            for (int j = 0; j < tasks_per_producer; ++j) {
                                    pool.submit([](atomic<int>& counter) {
                                                    counter.fetch_add(1, memory_order_relaxed);
                                                },ref(counter));
                            }
                        },ref(counter),ref(pool));
    }

    for (int i=0;i<producers;i++){
        th[i].join();
    }
    pool.wait();

    delete[] th;

    assert(counter == producers * tasks_per_producer);
    TEST_OK("test_multiple_producers");
}

void test_exceptions_in_tasks(ThreadPool& pool) {
    atomic<int> counter{0};

    for (int i = 0; i < 1000; ++i) {
        pool.submit([](atomic<int>& counter) {
            if (rand() % 10 == 0)
                throw runtime_error("boom");
            ++counter;
        },ref(counter));
    }

    // Si tu ThreadPool no captura excepciones,
    // este test puede matar threads silenciosamente
    pool.wait();
    TEST_OK("test_exceptions_in_tasks");
}

/* ============================================================
   DESTRUCTOR Y SHUTDOWN
   ============================================================ */

void test_destruction_with_pending_tasks() {
    {
        ThreadPool pool(4);
        for (int i = 0; i < 5000; ++i) {
            pool.submit([]() {
                this_thread::sleep_for(chrono::milliseconds(1));
            });
        }
    }
    TEST_OK("test_destruction_with_pending_tasks");
}

/* ============================================================
   PERFORMANCE
   ============================================================ */

void benchmark_throughput(ThreadPool& pool) {
    constexpr int N = 300000;
    atomic<int> counter{0};

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        pool.submit([&](atomic<int>& counter) {
            counter.fetch_add(1, memory_order_relaxed);
        },ref(counter));
    }

    pool.wait();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Throughput: "
         << (N / elapsed.count())
         << " tasks/sec" << endl;
}

void benchmark_scaling() {
    cout << "\nSCALING TEST\n";
    for (int threads : {1, 2, 4, 8}) {
        ThreadPool pool(threads);
        atomic<int> counter{0};

        auto start = chrono::steady_clock::now();

        for (int i = 0; i < 200000; ++i) {
            pool.submit([&](atomic<int>& counter) {
                counter.fetch_add(1, memory_order_relaxed);
            },ref(counter));
        }

        pool.wait();

        auto end = chrono::steady_clock::now();
        chrono::duration<double> t = end - start;

        cout << threads << " threads -> "
             << t.count() << " s" << endl;
    }
}

/* ============================================================
   BUSY WAITING (CPU IDLE)
   ============================================================ */

void test_idle_cpu_usage() {
    cout << "\nIdle CPU test (10s sin tareas)\n";
    ThreadPool pool(4);
    this_thread::sleep_for(chrono::seconds(10));
    TEST_OK("test_idle_cpu_usage");
}

/* ============================================================
   MAIN
   ============================================================ */

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    ThreadPool pool(4);

    test_basic_execution(pool);
    test_reuse_after_wait(pool);
    test_multiple_producers(pool);
    test_exceptions_in_tasks(pool);
    //
    benchmark_throughput(pool);
    //
    pool.finish();
    //
    test_destruction_with_pending_tasks();
    benchmark_scaling();
    test_idle_cpu_usage();

    return 0;
}
