CXXFLAGS=-I../include -Wall -g
LDFLAGS=-L.. -lcxxtools -ldl -lpthread
CC=g++

all: pooltest hdstreamtest cgitest query_params_test md5test query_params_it

hdstreamtest: hdstreamtest.o

cgitest: cgitest.o

deps::
	( for f in *.cpp; do $(CC) $(CXXFLAGS) -M $$f; done ) >Makedeps

include Makedeps
