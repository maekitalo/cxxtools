INCLUDES= -I$(top_builddir)/include

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

libcxxtools_unit_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

