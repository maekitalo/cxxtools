AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools-xml.la

libcxxtools_xml_la_SOURCES = \
	characters.cpp \
	endelement.cpp \
	entityresolver.cpp \
	namespacecontext.cpp \
	startelement.cpp \
	xmldeserializer.cpp \
	xmlerror.cpp \
	xmlformatter.cpp \
	xmlreader.cpp \
	xmlserializer.cpp \
	xmlwriter.cpp

libcxxtools_xml_la_LIBADD = $(top_builddir)/src/libcxxtools.la

libcxxtools_xml_la_LDFLAGS = -version-info @sonumber@

