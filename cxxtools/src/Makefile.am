lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	dlloader.cpp \
	dynbuffer.cpp \
	hdstream.cpp \
	iconvstream.cpp \
	iniclass.cpp \
	md5.c \
	md5stream.cpp \
	pollclass.cpp \
	query_params.cpp \
	tcpstream.cpp \
	tee.cpp \
	thread.cpp \
	timeclass.cpp

libcxxtools_la_LDFLAGS = -ldl -lpthread -version-info @sonumber@

INCLUDES=-I../include
