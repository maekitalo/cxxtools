CXXFLAGS=-fPIC -I../include -I../include/cxxtools -g -Wall -pedantic
CFLAGS=-fPIC -I../include -I../include/cxxtools -O2 -Wall
LDFLAGS=-ldl -lpthread
CC=g++

SRCS = dlloader.cpp dynbuffer.cpp hdstream.cpp \
       iniclass.cpp md5stream.cpp pollclass.cpp \
       query_params.cpp tcpstream.cpp tee.cpp \
       thread.cpp timeclass.cpp

OBJECTS=$(SRCS:.cpp=.o) md5.o

.PHONY: all clean Makefile

all: libcxxtools.so.$(VERSION)

clean:
	rm -rf *.o lib* depend

depend:
	( for f in $(SRCS) md5.c; do $(CC) $(CXXFLAGS) -MM $$f; done ) >depend

libcxxtools.a: $(OBJECTS)
	rm -f $@
	ar rcs $@ $^
	ranlib $@

libcxxtools.so.$(VERSION): $(OBJECTS)
	$(CC) -shared -Wl,-soname,libcxxtools.so.$(VERSION) $(LDFLAGS) -o libcxxtools.so.$(VERSION) $^


include depend
