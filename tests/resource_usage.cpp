#include <iostream>
#include <thread>
#include <chrono>
#include "../include/ThreadPool.hpp"

using namespace std;

/*
   Este test NO mide tiempo de ejecución,
   mide CUÁNTO CPU consume el threadpool SIN HACER NADA.
*/
void test_idle_resource_usage() {
    cout << "\n=== RESOURCE USAGE TEST (IDLE) ===\n";
    cout << "Creando ThreadPool con 4 threads...\n";

    ThreadPool pool(4);

    cout << "No se envían tareas durante 10 segundos.\n";
    cout << "Observa el uso de CPU del proceso.\n";

    auto start = chrono::steady_clock::now();
    this_thread::sleep_for(chrono::seconds(10));
    auto end = chrono::steady_clock::now();

    cout << "Tiempo real: "
         << chrono::duration_cast<chrono::seconds>(end - start).count()
         << " s\n";
}

/*
   Test con tareas mínimas para medir overhead
*/
void test_minimal_task_overhead() {
    cout << "\n=== RESOURCE USAGE TEST (MINIMAL TASKS) ===\n";

    ThreadPool pool(4);

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 100000; ++i) {
        pool.submit([](){

        });
    }

    pool.wait();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "100k empty tasks in "
         << elapsed.count() << " seconds\n";
}

int main() {
    cout<<1<<endl;
    test_idle_resource_usage();
    cout<<2<<endl;
    test_minimal_task_overhead();
    return 0;
}
