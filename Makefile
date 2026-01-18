CXX=g++
CXXFLAGS_PROD=-O3 -std=c++17 -Wall -Iinclude -fPIC -shared
CXXFLAGS_STAT=-O3 -c -std=c++11 -Iinclude
CXXFLAGS_DEB=-g -pg -std=c++17 -Wall -Iinclude
CXXFILES=src/test.cpp src/ThreadPool.cpp

CXXFILES=src/ThreadPool.cpp

all:
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


example: src/test.cpp include/test.hpp examples/example.cpp include/ThreadPool.hpp
	@echo "Compiling example.cpp into example"
	@${CXX} -o example ${CXXFLAGS_DEB} ${CXXFILES} examples/example.cpp


# Target for the creation of the documentation
documentation: src/ThreadPool.cpp include/ThreadPool.hpp
	@doxygen Doxyfile
	@cd docs/latex;make;mv refman.pdf ../doc.pdf
