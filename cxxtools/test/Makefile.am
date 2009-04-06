noinst_PROGRAMS = net-test xmlrpc-test

INCLUDES = -I$(top_builddir)/include

net_test_SOURCES = net-test.cpp tcpsockettest.cpp

xmlrpc_test_SOURCES = xmlrpc-test.cpp

LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(PTHREAD_LIBS)
