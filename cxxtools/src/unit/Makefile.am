AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools-unit.la

libcxxtools_unit_la_SOURCES = \
	application.cpp \
	testcase.cpp \
	test.cpp \
	testsuite.cpp \
	assertion.cpp \
	reporter.cpp \
	testcontext.cpp \
	testprotocol.cpp

libcxxtools_unit_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_unit_la_LDFLAGS = -version-info @sonumber@

