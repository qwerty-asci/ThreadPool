all: debug


CXX=g++
CXXFLAGS_PROD=-o main
CXXFLAGS_DEB=-g -o main -std=c++17


main: threadpool.cpp
	@echo "Main version"
	@${CXX} ${CXXFLAGS_PROD} threadpool.cpp


debug: threadpool.cpp
	@echo "Debuging version"
	@${CXX} ${CXXFLAGS_DEB} threadpool.cpp



test: test.cpp
	@echo "Debuging version"
	@${CXX} ${CXXFLAGS_DEB} test.cpp
