INCLUDES= -I$(top_builddir)/include

lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	base64stream.cpp \
	condition.cpp \
	connectable.cpp \
	connection.cpp \
	cgi.cpp \
	convert.cpp \
	directory.cpp \
	directoryimpl.cpp \
	fdstream.cpp \
	file.cpp \
	fileimpl.cpp \
	fileinfo.cpp \
	hdstream.cpp \
	hirestime.cpp \
	httpreply.cpp \
	httprequest.cpp \
	iconvstream.cpp \
	inifile.cpp \
	iniparser.cpp \
	ioerror.cpp \
	library.cpp \
	libraryimpl.cpp \
	log.cpp \
	md5.c \
	md5stream.cpp \
	mime.cpp \
	multifstream.cpp \
	mutex.cpp \
	muteximpl.cpp \
	net.cpp \
	pipe.cpp \
	pipestream.cpp \
	pollclass.cpp \
	properties.cpp \
	query_params.cpp \
	quotedprintablestream.cpp \
	regex.cpp \
	signal.cpp \
	syserror.cpp \
	systemerror.cpp \
	tcpstream.cpp \
	tee.cpp \
	thread.cpp \
	threadimpl.cpp \
	udp.cpp \
	udpstream.cpp \
	uuencode.cpp \
	xmltag.cpp

noinst_HEADERS = \
	directoryimpl.h \
	fileimpl.h \
	fileinfoimpl.h

if MAKE_ATOMICITY_PTHREAD
libcxxtools_la_SOURCES += \
	atomicity.cpp
endif

libcxxtools_la_LIBADD = $(DLLIBADD) $(LIBICONV)
libcxxtools_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libcxxtools_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

