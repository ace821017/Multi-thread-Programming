CXX := g++
CXXFLAGS := -I./common -O3 -std=c++17 -Wall

ifeq (/usr/bin/g++-10,$(wildcard /usr/bin/g++-10*))
    CXX=g++-10
endif

all: pii

pii: pi.cpp 
	g++  -I./common pi.cpp -o pi.out