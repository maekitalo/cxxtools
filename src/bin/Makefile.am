AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools-bin.la

noinst_HEADERS = \
	responder.h \
	rpcclientimpl.h \
	rpcserverimpl.h \
	scanner.h \
	socket.h \
	worker.h

libcxxtools_bin_la_SOURCES = \
	deserializer.cpp \
	formatter.cpp \
	responder.cpp \
	socket.cpp \
	rpcclient.cpp \
	rpcclientimpl.cpp \
	rpcserver.cpp \
	rpcserverimpl.cpp \
	serializer.cpp \
	scanner.cpp \
	parser.cpp \
	worker.cpp

libcxxtools_bin_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_bin_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

