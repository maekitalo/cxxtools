noinst_PROGRAMS = \
    abort \
    alltests \
    arg-test \
    base64-test \
    cache-test \
    convert-test \
    net-test \
    pool-test \
    process-test \
    smartptr-test \
    uri-test \
    xmlreader-test \
    xmlrpc-test

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

abort_SOURCES = abort.cpp

alltests_SOURCES = \
    arg-test.cpp \
    base64-test.cpp \
    cache-test.cpp \
    convert-test.cpp \
    pool-test.cpp \
    smartptr-test.cpp \
    test-main.cpp \
    trim-test.cpp \
    uri-test.cpp \
    xmlreader-test.cpp \
    xmlrpc-test.cpp

arg_test_SOURCES = test-main.cpp arg-test.cpp

base64_test_SOURCES = test-main.cpp base64-test.cpp

cache_test_SOURCES = test-main.cpp cache-test.cpp

convert_test_SOURCES = test-main.cpp convert-test.cpp

net_test_SOURCES = test-main.cpp tcpsockettest.cpp

process_test_SOURCES = test-main.cpp process-test.cpp
process_test_CPPFLAGS = -I$(top_builddir)/include -DTOP_BUILDDIR='"$(abs_top_builddir)"'

pool_test_SOURCES = test-main.cpp pool-test.cpp

smartptr_test_SOURCES = test-main.cpp smartptr-test.cpp

uri_test_SOURCES = test-main.cpp uri-test.cpp

xmlreader_test_SOURCES = test-main.cpp xmlreader-test.cpp

xmlrpc_test_SOURCES = test-main.cpp xmlrpc-test.cpp

LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xml/libcxxtools-xml.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
AM_CXXFLAGS = $(PTHREAD_CFLAGS) -DTESTDIR='"@TESTDIR@"'
AM_LDFLAGS = $(PTHREAD_LIBS)
