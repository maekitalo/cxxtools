INCLUDES= -I$(top_builddir)/include

lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	any.cpp \
	atomicity.cpp \
	base64stream.cpp \
	connectable.cpp \
	connection.cpp \
	cgi.cpp \
	dlloader.cpp \
	fdstream.cpp \
	hdstream.cpp \
	hirestime.cpp \
	httpreply.cpp \
	httprequest.cpp \
	iconvstream.cpp \
	inifile.cpp \
	iniparser.cpp \
	log.cpp \
	md5.c \
	md5stream.cpp \
	multifstream.cpp \
	net.cpp \
	pipe.cpp \
	pipestream.cpp \
	pollclass.cpp \
	properties.cpp \
	query_params.cpp \
	syserror.cpp \
	tcpstream.cpp \
	tee.cpp \
	thread.cpp \
	udp.cpp \
	udpstream.cpp \
	xmltag.cpp

libcxxtools_la_LIBADD = $(DLLIBADD)
libcxxtools_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libcxxtools_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@ -ldl

