INCLUDES= -I$(top_builddir)/include

lib_LTLIBRARIES = libcxxtools-http.la

libcxxtools_http_la_SOURCES = \
	client.cpp \
	messageheader.cpp \
	parser.cpp \
	server.cpp \
	socket.cpp \
	responder.cpp

libcxxtools_http_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_http_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

