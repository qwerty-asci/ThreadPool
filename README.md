# ThreadPool

A simple C++ thread pool implementation that allows executing tasks concurrently using a fixed number of worker threads.

This project is intended for educational purposes and demonstrates the basic concepts behind thread pools, task queues, and multithreading using the C++ Standard Library.

---

## Features

- Fixed-size pool of worker threads
- Task submission using callable objects (`std::function`)
- Thread-safe task queue protected by a mutex
- Graceful shutdown with optional thread joining
- Doxygen-documented source code

---

## Project Structure

ThreadPool
├── docs
│   ├── html
│   │   └── index.html
│   └── latex
│       └── refman.pdf
├── Doxyfile
├── examples
│   └── main.cpp
├── include
│   ├── test.hpp
│   └── ThreadPool.hpp
├── LICENSE
├── main
├── Makefile
├── README.md
├── refman.pdf
├── src
│   ├── test.cpp
│   └── ThreadPool.cpp
└── tests




## Requirements

- C++11 or later
- A compiler with support for `<thread>` (e.g. `g++`, `clang++`)
- (Optional) Doxygen for documentation generation
- (Optional) Graphviz for diagrams in documentation

---

<!--## Building

This project does not require a build system by default.

Example compilation using `g++`:

```bash
g++ -std=c++11 -Iinclude src/ThreadPool.cpp examples/basic_example.cpp -o example-->
