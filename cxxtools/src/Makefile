CXXFLAGS=-fPIC -I../include -I../include/cxxtools -g -Wall -pedantic
CFLAGS=-fPIC -I../include -I../include/cxxtools -O2 -Wall
LDFLAGS=
CC=g++

SRCS = dlloader.cpp dynbuffer.cpp hdstream.cpp \
       iniclass.cpp md5stream.cpp pollclass.cpp \
       query_params.cpp tcpstream.cpp tee.cpp \
       thread.cpp timeclass.cpp

OBJECTS=$(SRCS:.cpp=.o)

.PHONY: all clean libcxxtools.a libcxxtools.so Makefile

all: libcxxtools.so

clean:
	rm -rf *.o lib* depend

depend:
	( for f in *.cpp; do $(CC) $(CXXFLAGS) -MM $$f; done ) >depend

libcxxtools.a: $(OBJECTS)
	rm -f $@
	ar rcs $@ $^
	ranlib $@

libcxxtools.so: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libcxxtools.so.$(VERSION) -o libcxxtools.so.$(VERSION) $^


include depend
