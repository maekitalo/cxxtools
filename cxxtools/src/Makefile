CXXFLAGS=-fPIC -I../include -I../include/cxxtools -g -Wall -pedantic
CFLAGS=-fPIC -I../include -I../include/cxxtools -O2 -Wall
LDFLAGS=
CC=g++
SRCS = *.cpp
OBJECTS=iniclass.o pollclass.o tcpstream.o timeclass.o \
    dynbuffer.o thread.o tcpstream.o timeclass.o hdstream.o \
    dlloader.o query_params.o tee.o md5.o md5stream.o

.PHONY: all clean libcxxtools.a libcxxtools.so Makefile $(SRCS)

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
