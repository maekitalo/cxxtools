AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools-xmlrpc.la

libcxxtools_xmlrpc_la_SOURCES = \
    client.cpp \
    clientimpl.cpp \
    httpclient.cpp \
    httpclientimpl.cpp \
	fault.cpp \
	formatter.cpp \
	remoteprocedure.cpp \
	responder.cpp \
	scanner.cpp \
	service.cpp

noinst_HEADERS = \
    clientimpl.h \
    httpclientimpl.h

libcxxtools_xmlrpc_la_LIBADD = $(top_builddir)/src/http/libcxxtools-http.la \
	$(top_builddir)/src/xml/libcxxtools-xml.la

libcxxtools_xmlrpc_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

