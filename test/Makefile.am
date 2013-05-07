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
    clock-test.cpp \
    csvdeserializer-test.cpp \
    csvserializer-test.cpp \
    convert-test.cpp \
    join-test.cpp \
    json-test.cpp \
    jsondeserializer-test.cpp \
    jsonrpc-test.cpp \
    jsonrpchttp-test.cpp \
    jsonserializer-test.cpp \
    lrucache-test.cpp \
    md5-test.cpp \
    pool-test.cpp \
    properties-test.cpp \
    query_params-test.cpp \
    regex-test.cpp \
    serializationinfo-test.cpp \
    smartptr-test.cpp \
    split-test.cpp \
    string-test.cpp \
    test-main.cpp \
    trim-test.cpp \
    uri-test.cpp \
    xmlreader-test.cpp \
    xmlrpc-test.cpp \
    xmlrpccallback-test.cpp \
    xmlserializer-test.cpp

if MAKE_ICONVSTREAM
alltests_SOURCES += \
	iconvstream-test.cpp
endif

alltests_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/json/libcxxtools-json.la \
        $(top_builddir)/src/unit/libcxxtools-unit.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la

serializer_bench_SOURCES = serializer-bench.cpp

serializer_bench_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la

rpcbenchclient_SOURCES = rpcbenchclient.cpp

rpcbenchclient_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la \
        $(top_builddir)/src/json/libcxxtools-json.la

rpcbenchserver_SOURCES = rpcbenchserver.cpp

rpcbenchserver_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la \
        $(top_builddir)/src/json/libcxxtools-json.la
