AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools-json.la

noinst_HEADERS = \
	rpcclientimpl.h \
	listener.h \
	responder.h \
	rpcserverimpl.h \
	scanner.h \
	socket.h \
	worker.h

libcxxtools_json_la_SOURCES = \
	rpcclient.cpp \
	rpcclientimpl.cpp \
	responder.cpp \
	rpcserver.cpp \
	rpcserverimpl.cpp \
	scanner.cpp \
	socket.cpp \
	worker.cpp

libcxxtools_json_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_json_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

