noinst_PROGRAMS = \
    alltests \
	serializer-bench \
	rpcbenchclient \
	rpcbenchserver

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

alltests_SOURCES = \
    arg-test.cpp \
    base64-test.cpp \
    binrpc-test.cpp \
    binserializer-test.cpp \
    cache-test.cpp \
    csvdeserializer-test.cpp \
    csvserializer-test.cpp \
    convert-test.cpp \
    jsondeserializer-test.cpp \
    md5-test.cpp \
    pool-test.cpp \
    serializationinfo-test.cpp \
    smartptr-test.cpp \
    string-test.cpp \
    test-main.cpp \
    trim-test.cpp \
    uri-test.cpp \
    xmlreader-test.cpp \
    xmlrpc-test.cpp

alltests_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xml/libcxxtools-xml.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la

serializer_bench_SOURCES = serializer-bench.cpp

serializer_bench_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la \
        $(top_builddir)/src/xml/libcxxtools-xml.la

rpcbenchclient_SOURCES = rpcbenchclient.cpp

rpcbenchclient_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la

rpcbenchserver_SOURCES = rpcbenchserver.cpp

rpcbenchserver_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la

AM_CXXFLAGS = $(PTHREAD_CFLAGS) -DTESTDIR='"@TESTDIR@"'
AM_LDFLAGS = $(PTHREAD_LIBS)
