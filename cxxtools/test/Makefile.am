noinst_PROGRAMS = \
    alltests \
	serializer-bench \
	rpcbenchclient \
	rpcbenchserver

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

alltests_SOURCES = \
    arg-test.cpp \
    base64-test.cpp \
    binserializer-test.cpp \
    cache-test.cpp \
    convert-test.cpp \
    pool-test.cpp \
    smartptr-test.cpp \
    string-test.cpp \
    test-main.cpp \
    trim-test.cpp \
    uri-test.cpp \
    xmlreader-test.cpp \
    xmlrpc-test.cpp

serializer_bench_SOURCES = serializer-bench.cpp
rpcbenchclient_SOURCES = rpcbenchclient.cpp
rpcbenchserver_SOURCES = rpcbenchserver.cpp

LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xml/libcxxtools-xml.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
AM_CXXFLAGS = $(PTHREAD_CFLAGS) -DTESTDIR='"@TESTDIR@"'
AM_LDFLAGS = $(PTHREAD_LIBS)
