CPP=g++
CPPFLAGS=-lpthread -lboost_system

.PHONY: all clean indent

all: build build/threads_thread build/threads_async build/server_singlethreaded build/server_multithreaded_thread build/client build/server_multithreaded_via_singleton

build:
	mkdir build

build/%: %.cc
	${CPP} -std=c++11 -o $@ $< ${CPPFLAGS}

clean:
	rm -rf build

indent:
	find . -regextype posix-egrep -regex ".*\.(cc|h)" | xargs clang-format-3.5 -i
