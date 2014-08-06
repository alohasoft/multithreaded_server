CPP=g++
CPPFLAGS=-lpthread -lboost_system

.PHONY: all clean indent

all: build build/threads_thread build/threads_async build/server_singlethreaded build/server_multithreaded

build:
	mkdir build

build/%: %.cc
	${CPP} -std=c++11 -o $@ $< ${CPPFLAGS}

clean:
	rm -rf build

indent:
	find . -iname "*.cc" | xargs clang-format-3.5 -i
