AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools-bin.la

libcxxtools_bin_la_SOURCES = \
	deserializer.cpp \
	formatter.cpp \
	valueparser.cpp

libcxxtools_bin_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_bin_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

