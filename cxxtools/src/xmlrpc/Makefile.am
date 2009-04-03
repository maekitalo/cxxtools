INCLUDES= -I$(top_builddir)/include

lib_LTLIBRARIES = libcxxtools-xmlrpc.la

libcxxtools_xmlrpc_la_SOURCES = client.cpp \
	fault.cpp \
	formatter.cpp \
	remoteprocedure.cpp \
	responder.cpp \
	scanner.cpp \
	service.cpp

libcxxtools_xmlrpc_la_LIBADD = $(top_builddir)/src/libcxxtools.la \
	$(top_builddir)/src/xml/libcxxtools-xml.la

libcxxtools_xmlrpc_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

