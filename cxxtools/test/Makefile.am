noinst_PROGRAMS = \
    net-test \
    xmlrpc-test \
    process-test \
    abort \
    base64-test \
    arg-test \
    convert-test \
    alltests

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

net_test_SOURCES = test-main.cpp tcpsockettest.cpp

xmlrpc_test_SOURCES = test-main.cpp xmlrpc-test.cpp

process_test_SOURCES = test-main.cpp process-test.cpp
process_test_CPPFLAGS = -I$(top_builddir)/include -DTOP_BUILDDIR='"$(abs_top_builddir)"'

base64_test_SOURCES = test-main.cpp base64-test.cpp

arg_test_SOURCES = test-main.cpp arg-test.cpp

convert_test_SOURCES = test-main.cpp convert-test.cpp

alltests_SOURCES = \
    arg-test.cpp \
    base64-test.cpp \
    tcpsockettest.cpp \
    test-main.cpp \
    process-test.cpp \
    xmlrpc-test.cpp \
    convert-test.cpp

abort_SOURCES = abort.cpp

LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xml/libcxxtools-xml.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
AM_CXXFLAGS = $(PTHREAD_CFLAGS) -DTESTDIR='"@TESTDIR@"'
AM_LDFLAGS = $(PTHREAD_LIBS)
