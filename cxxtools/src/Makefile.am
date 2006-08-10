INCLUDES = -I$(top_builddir)/include @INCLTDL@

lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	base64stream.cpp \
	cgi.cpp \
	dlloader.cpp \
	hdstream.cpp \
	hirestime.cpp \
	httpreply.cpp \
	httprequest.cpp \
	iniclass.cpp \
	log.cpp \
	md5.c \
	md5stream.cpp \
	multifstream.cpp \
	net.cpp \
	pollclass.cpp \
	query_params.cpp \
	tcpstream.cpp \
	tee.cpp \
	thread.cpp \
	udp.cpp \
	udpstream.cpp \
	xmltag.cpp

if MAKE_ICONV
libcxxtools_la_SOURCES += iconv.cpp iconvstream.cpp
endif

libcxxtools_la_LIBADD = $(MAKE_ICONV) @LIBLTDL@ @LIBICONV@
libcxxtools_la_DEPENDENCIES = $(MAKE_ICONV)
libcxxtools_la_LDFLAGS = -lpthread -version-info @sonumber@
