all: debug


CXX=g++
CXXFLAGS_PROD=-o main
CXXFLAGS_DEB=-g -o main -std=c++11


main: threadpool.cpp
	@echo "Main version"
	@${CXX} ${CXXFLAGS_PROD} threadpool.cpp


debug: threadpool.cpp
	@echo "Debuging version"
	@${CXX} ${CXXFLAGS_DEB} threadpool.cpp
