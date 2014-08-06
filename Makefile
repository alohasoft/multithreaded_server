CPP=g++
CPPFLAGS=-lpthread

.PHONY: all clean indent

all: build build/threads

build:
	mkdir build

build/%: %.cc
	${CPP} -std=c++11 -o $@ $< ${CPPFLAGS}

clean:
	rm -rf build

indent:
	find . -iname *.cc | xargs clang-format-3.5 -i
