CXXFLAGS=-fPIC -I../include -I../include/cxxtools -O2 -Wall -pedantic
CFLAGS=-fPIC -I../include -I../include/cxxtools -O2 -Wall
LDFLAGS=-ldl -lpthread
CC=g++
ifeq ($(EP_BUILD_COMP_VER), GCC_3_0)
  CC=g++-3.0
endif

ifeq ($(EP_BUILD_COMP_VER), GCC_2_95)
  CC=g++-2.95
endif

SRCS = dlloader.cpp dynbuffer.cpp hdstream.cpp \
       iniclass.cpp md5stream.cpp pollclass.cpp \
       query_params.cpp tcpstream.cpp tee.cpp \
       thread.cpp timeclass.cpp iconvstream.cpp

OBJECTS=$(SRCS:.cpp=.o) md5.o

.PHONY: all clean Makefile

all: libcxxtools.so.$(VERSION)

clean:
	rm -rf *.o lib* depend

depend: $(SRCS)
	( for f in $(SRCS) md5.c; do $(CC) $(CXXFLAGS) -M $$f; done ) >depend

libcxxtools.a: $(OBJECTS)
	rm -f $@
	ar rcs $@ $^
	ranlib $@

libcxxtools.so.$(VERSION): $(OBJECTS)
	$(CC) -shared -Wl,-soname,libcxxtools.so.$(VERSION) $(LDFLAGS) -o libcxxtools.so.$(VERSION) $^

%.o: %.cpp
	@echo "--- C++ Compiling $<..."
	$(CreateOutputDirectory)
	$(CC) $(CXXFLAGS) -c $< -o $@


include depend
