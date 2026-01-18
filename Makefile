all: debug


CXX=g++
CXXFLAGS_PROD=-o main
CXXFLAGS_DEB=-g -o main -std=c++17 -Wall
CXXFILES=test.cpp ThreadPool.cpp


main: ThreadPool.cpp
	@echo "Main version"
	@${CXX} ${CXXFLAGS_PROD} main.cpp


debug: ThreadPool.cpp main.cpp test.cpp
	@echo "Debuging version"
	@${CXX} ${CXXFLAGS_DEB} ${CXXFILES} main.cpp



test: test.cpp
	@echo "Debuging version"
	@${CXX} ${CXXFLAGS_DEB} test.cpp

documentation: src/ThreadPool.cpp include/ThreadPool.hpp
	@doxygen Doxyfile
	@cd docs/latex;make;mv refman.pdf ../../
