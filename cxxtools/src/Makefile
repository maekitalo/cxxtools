VERSION=1.0.0
CXXFLAGS = -Wall -O2 -I../include -fPIC $(LOCAL_CXXFLAGS)
CFLAGS = -Wall -O2 -I../include -fPIC
LDFLAGS = -ldl -lpthread $(LOCAL_CXXFLAGS)

all : libcxxtools.so

SRCS = \
	dlloader.cpp \
	dynbuffer.cpp \
	hdstream.cpp \
	iconvstream.cpp \
	md5stream.cpp \
	pollclass.cpp \
	query_params.cpp \
	tcpstream.cpp \
	tee.cpp \
	thread.cpp \
	timeclass.cpp

SRCS_OLD = iniclass.cpp

SRCS_C = md5.c

OBJS = $(SRCS:.cpp=.o) $(SRCS_C:.c=.o)
DEPS = $(SRCS:.cpp=.deps) $(SRCS_C:.c=.deps)

%.deps : %.cpp
	( echo -n $(dir $<) && $(CXX) $(CXXFLAGS) -M $< || ( rm -f $@; exit 1 )) >$@

%.deps : %.c
	( echo -n $(dir $<) && $(CC) $(CFLAGS) -M $< || ( rm -f $@; exit 1 )) >$@

libcxxtools.so.$(VERSION) : $(OBJS)
	$(CXX) -shared -o $@ $(LDFLAGS) $^

libcxxtools.so: libcxxtools.so.$(VERSION)
	ln -sf $< $@

install: libcxxtools.so.$(VERSION)
	cp $^ $(DESTDIR)/usr/lib
	ln -sf libcxxtools.so.$(VERSION) $(DESTDIR)/usr/lib/libcxxtools.so

clean:
	rm -f $(OBJS) $(DEPS) libcxxtools.so libcxxtools.so.$(VERSION)

.PHONY: depend
depend: $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
