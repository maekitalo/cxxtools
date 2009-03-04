noinst_PROGRAMS = net-test 

INCLUDES = -I$(top_builddir)/include

net_test_SOURCES = net-test.cpp tcpsockettest.cpp

LDADD = $(top_builddir)/src/libcxxtools.la $(top_builddir)/src/unit/libcxxtools-unit.la
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(PTHREAD_LIBS)
