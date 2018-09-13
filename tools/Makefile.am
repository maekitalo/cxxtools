bin_PROGRAMS = \
	siconvert

siconvert_SOURCES = siconvert.cpp

BASE_LIBS = $(top_builddir)/src/libcxxtools.la
HTTP_LIBS = $(BASE_LIBS) $(top_builddir)/src/http/libcxxtools-http.la
XMLRPC_LIBS = $(HTTP_LIBS) $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
BIN_LIBS = $(BASE_LIBS) $(top_builddir)/src/bin/libcxxtools-bin.la
JSON_LIBS = $(HTTP_LIBS) $(top_builddir)/src/json/libcxxtools-json.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include
LDADD = $(top_builddir)/src/libcxxtools.la

siconvert_LDADD = $(BIN_LIBS)
