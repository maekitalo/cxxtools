INCLUDES= -I$(top_builddir)/include

lib_LTLIBRARIES = libcxxtools-http.la

libcxxtools_http_la_SOURCES = \
    chunkedreader.cpp \
    client.cpp \
    clientimpl.cpp \
    messageheader.cpp \
    notauthenticatedresponder.cpp \
    notauthenticatedservice.cpp \
    notfoundresponder.cpp \
    notfoundservice.cpp \
    parser.cpp \
    server.cpp \
    serverimpl.cpp \
    socket.cpp \
    request.cpp \
    responder.cpp \
    worker.cpp

noinst_HEADERS = \
    chunkedreader.h \
    clientimpl.h \
    listener.h \
    notauthenticatedresponder.h \
    notauthenticatedservice.h \
    notfoundresponder.h \
    notfoundservice.h \
    parser.h \
    serverimpl.h \
    socket.h \
    worker.h

libcxxtools_http_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_http_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

