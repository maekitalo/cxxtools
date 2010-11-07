noinst_PROGRAMS = \
    abort \
    alltests \
    arg-test \
    base64-test \
    convert-test \
    net-test \
    process-test \
    uri-test \
    xmlrpc-test

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

net_test_SOURCES = test-main.cpp tcpsockettest.cpp

uri_test_SOURCES = test-main.cpp uri-test.cpp

xmlrpc_test_SOURCES = test-main.cpp xmlrpc-test.cpp

process_test_SOURCES = test-main.cpp process-test.cpp
process_test_CPPFLAGS = -I$(top_builddir)/include -DTOP_BUILDDIR='"$(abs_top_builddir)"'

base64_test_SOURCES = test-main.cpp base64-test.cpp

arg_test_SOURCES = test-main.cpp arg-test.cpp

convert_test_SOURCES = test-main.cpp convert-test.cpp

alltests_SOURCES = \
    arg-test.cpp \
    base64-test.cpp \
    convert-test.cpp \
    tcpsockettest.cpp \
    test-main.cpp \
    trim-test.cpp \
    uri-test.cpp \
    xmlrpc-test.cpp

abort_SOURCES = abort.cpp

LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xml/libcxxtools-xml.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
AM_CXXFLAGS = $(PTHREAD_CFLAGS) -DTESTDIR='"@TESTDIR@"'
AM_LDFLAGS = $(PTHREAD_LIBS)
