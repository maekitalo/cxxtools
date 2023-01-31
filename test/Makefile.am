noinst_PROGRAMS = \
    alltests \
    logbench \
    serializer-bench \
    rpcbenchclient \
    rpcbenchasyncclient \
    rpcbenchserver

noinst_HEADERS = \
    color.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

alltests_SOURCES = \
    arg-test.cpp \
    base64-test.cpp \
    binrpc-test.cpp \
    binserializer-test.cpp \
    cache-test.cpp \
    char-test.cpp \
    clock-test.cpp \
    csvdeserializer-test.cpp \
    csvserializer-test.cpp \
    convert-test.cpp \
    date-test.cpp \
    datetime-test.cpp \
    directory-test.cpp \
    envsubst-test.cpp \
    eventloop-test.cpp \
    file-test.cpp \
    fileinfo-test.cpp \
    inifile-test.cpp \
    iniparser-test.cpp \
    iniserialization-test.cpp \
    iso8859_1-test.cpp \
    iso8859_15-test.cpp \
    join-test.cpp \
    json-test.cpp \
    jsondeserializer-test.cpp \
    jsonrpc-test.cpp \
    jsonrpchttp-test.cpp \
    jsonserializer-test.cpp \
    limitstream-test.cpp \
    logconfiguration-test.cpp \
    lrucache-test.cpp \
    mime-test.cpp \
    md5-test.cpp \
    pool-test.cpp \
    properties-test.cpp \
    propertiesserializer-test.cpp \
    ptrstream-test.cpp \
    query_params-test.cpp \
    quotedprintable-test.cpp \
    regex-test.cpp \
    scopedincrement-test.cpp \
    serialization-test.cpp \
    serializationinfo-test.cpp \
    split-test.cpp \
    string-test.cpp \
    test-main.cpp \
    time-test.cpp \
    timespan-test.cpp \
    trim-test.cpp \
    tz-test.cpp \
    utf8-test.cpp \
    uri-test.cpp \
    win1252-test.cpp \
    xmlreader-test.cpp \
    xmlrpc-test.cpp \
    xmlrpccallback-test.cpp \
    xmldeserializer-test.cpp \
    xmlserializer-test.cpp

if MAKE_ICONVSTREAM
alltests_SOURCES += \
	iconvstream-test.cpp
endif

logbench_SOURCES = logbench.cpp

logbench_LDADD = $(top_builddir)/src/libcxxtools.la

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
rpcbenchasyncclient_SOURCES = rpcbenchasyncclient.cpp

rpcbenchclient_LDADD = $(top_builddir)/src/libcxxtools.la \
        $(top_builddir)/src/http/libcxxtools-http.la \
        $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la \
        $(top_builddir)/src/bin/libcxxtools-bin.la \
        $(top_builddir)/src/json/libcxxtools-json.la

rpcbenchasyncclient_LDADD = $(top_builddir)/src/libcxxtools.la \
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
