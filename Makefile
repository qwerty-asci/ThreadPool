CXX=g++
CXXFLAGS_PROD=-O3 -std=c++17 -Wall -Iinclude -fPIC -shared
CXXFLAGS_STAT=-O3 -c -std=c++11 -Iinclude
CXXFLAGS_DEB=-g -pg -std=c++17 -Wall -Iinclude
CXXFILES_EXAMPLE=src/test.cpp src/ThreadPool.cpp

CXXFILES=src/ThreadPool.cpp


CXXFLAGS_TEST=-g -pg -std=c++17 -Wall -Iinclude

#Sanitazers to test the code
#-> address to checks memory leaks, use-after-free, double free and buffer overflow
#-> undefined checks for undefined errors and overflows
#-> thread detecst race conditions, simultaneous writting and acces without mutex

CXXSANITAIZERS_MEMORY= -fsanitize=address,undefined
CXXSANITAIZERS_THREADS= -fsanitize=thread
CXXFILES_TEST=src/ThreadPool.cpp

all: shared documentation


shared:
	@echo "Creating a shared library"
	@mkdir -p lib
	@echo "Result saved on lib"
	@${CXX} ${CXXFLAGS_PROD} ${CXXFILES} -o lib/ThreadPool.so

static:
	@echo "Creating a static library"
	@mkdir -p lib build
	@echo "Result saved on lib"
	@${CXX} ${CXXFLAGS_STAT} ${CXXFILES} -o build/ThreadPool.o
	@ar rcs lib/libthreadpool.a build/ThreadPool.o


example: src/test.cpp include/test.hpp examples/example.cpp include/ThreadPool.hpp src/ThreadPool.cpp
	@echo "Compiling example.cpp into example"
	@${CXX} -o example ${CXXFLAGS_DEB} ${CXXFILES_EXAMPLE} examples/example.cpp


test:include/ThreadPool.hpp src/ThreadPool.cpp tests/threadpool_test.cpp
	@echo "Preparing tests..."
	@mkdir -p build/tests
	@${CXX} -o build/tests/test ${CXXFLAGS_TEST} ${CXXFILES_TEST} tests/threadpool_test.cpp
	@${CXX} -o build/tests/test_memory ${CXXFLAGS_TEST} ${CXXSANITAIZERS_MEMORY} ${CXXFILES_TEST} tests/threadpool_test.cpp
	@${CXX} -o build/tests/test_threads ${CXXFLAGS_TEST} ${CXXSANITAIZERS_THREADS} ${CXXFILES_TEST} tests/threadpool_test.cpp
	@echo "Test created in build"

run_profile:build/tests/test
	@echo "Profiling test..."
	@./build/tests/test
	@gprof build/tests/test build/tests/gmon.out > profile_results.txt
	@echo "Profile finished -> build/tests/profile_results.txt"

run_memory_test:build/tests/test_memory
	@echo "Memory and undefined behavior test starting"
	@./build/tests/test_memory > stdout_test_memory.txt 2> stderr_test_memory.txt
	@echo "Test finished finished. Check stderr_test_memory.txt"

run_thread_test:build/tests/test_threads
	@echo "Thread test starting"
	@./build/tests/test_threads > stdout_test_thread.txt 2> stderr_test_thread.txt
	@echo "Test finished finished. Check stderr_test_thread.txt"


# Target for the creation of the documentation
documentation: src/ThreadPool.cpp include/ThreadPool.hpp
	@doxygen -q Doxyfile > /dev/null 2>&1
	@cd docs/latex;make;mv refman.pdf ../doc.pdf
clean:
	@rm example
