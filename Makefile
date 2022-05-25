CC=gcc
CXX=g++
RM=rm -f

CPPFLAGS= -Iinclude -g
LDFLAGS=-rdynamic -export-dynamic
LIBS=

FLAGS ?=

SRCS=$(wildcard *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

.PHONY: all, run, clean, distclean

all: outdir bpfpack

run: all
	cd bin && ./bpfpack $(FLAGS)

outdir:
	[ -d bin ] || mkdir -p bin

bpfpack: $(OBJS)
	$(CXX) $(LDFLAGS) -o bin/bpfpack $(OBJS) $(LIBS)

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) -rd bin
