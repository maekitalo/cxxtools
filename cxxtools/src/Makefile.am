AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	addrinfo.cpp \
	addrinfoimpl.cpp \
	application.cpp \
	applicationimpl.cpp \
	char.cpp \
	clock.cpp \
	clockimpl.cpp \
	condition.cpp \
	conditionimpl.cpp \
	connectable.cpp \
	connection.cpp \
	cgi.cpp \
	conversionerror.cpp \
	date.cpp \
	datetime.cpp \
	deserializer.cpp \
	directory.cpp \
	directoryimpl.cpp \
	error.cpp \
	eventloop.cpp \
	eventsink.cpp \
	eventsource.cpp \
	fdstream.cpp \
	file.cpp \
	fileimpl.cpp \
	fileinfo.cpp \
	hdstream.cpp \
	inifile.cpp \
	iniparser.cpp \
	iodevice.cpp \
	iodeviceimpl.cpp \
	ioerror.cpp \
	iostream.cpp \
	jsonserializer.cpp \
	library.cpp \
	libraryimpl.cpp \
	log.cpp \
	md5.c \
	md5stream.cpp \
	mime.cpp \
	multifstream.cpp \
	mutex.cpp \
	muteximpl.cpp \
	pipe.cpp \
	pipeimpl.cpp \
	posix/pipestream.cpp \
	posix/posixpipe.cpp \
	process.cpp \
	processimpl.cpp \
	properties.cpp \
	query_params.cpp \
	quotedprintablestream.cpp \
	regex.cpp \
	selectable.cpp \
	selector.cpp \
	selectorimpl.cpp \
	semaphore.cpp \
	semaphoreimpl.cpp \
	settings.cpp \
	settingsreader.cpp \
	settingswriter.cpp \
	serializationerror.cpp \
	serializationinfo.cpp \
	serializer.cpp \
	signal.cpp \
	streambuffer.cpp \
	stringstream.cpp \
	systemerror.cpp \
	tee.cpp \
	textbuffer.cpp \
	textcodec.cpp \
	textstream.cpp \
	thread.cpp \
	threadimpl.cpp \
	time.cpp \
	timer.cpp \
	timespan.cpp \
	utf8codec.cpp \
	uuencode.cpp \
	xmltag.cpp \
	net.cpp \
	tcpserverimpl.cpp \
	tcpserver.cpp \
	tcpsocket.cpp \
	tcpsocketimpl.cpp \
	tcpstream.cpp \
	udp.cpp \
	udpstream.cpp

noinst_HEADERS = \
	addrinfoimpl.h \
	applicationimpl.h \
	clockimpl.h \
	conditionimpl.h \
	directoryimpl.h \
	error.h \
	fileimpl.h \
	fileinfoimpl.h \
	facets.cpp \
	iodeviceimpl.h \
	libraryimpl.h \
	muteximpl.h \
	pipeimpl.h \
	processimpl.h \
	selectableimpl.h \
	selectorimpl.h \
	semaphoreimpl.h \
	settingsreader.h \
	settingswriter.h \
	threadimpl.h \
	unicode.h \
	tcpserverimpl.h \
	tcpsocketimpl.h

if MAKE_ICONVSTREAM
libcxxtools_la_SOURCES += \
	iconvstream.cpp
endif

if MAKE_ATOMICITY_SUN
libcxxtools_la_SOURCES += \
	atomicity.sun.cpp
endif

if MAKE_ATOMICITY_WINDOWS
libcxxtools_la_SOURCES += \
	atomicity.windows.cpp
endif

if MAKE_ATOMICITY_GCC_ARM
libcxxtools_la_SOURCES += \
	atomicity.gcc.arm.cpp
endif

if MAKE_ATOMICITY_GCC_MIPS
libcxxtools_la_SOURCES += \
	atomicity.gcc.mips.cpp
endif

if MAKE_ATOMICITY_GCC_PPC
libcxxtools_la_SOURCES += \
	atomicity.gcc.ppc.cpp
endif

if MAKE_ATOMICITY_GCC_X86_64
libcxxtools_la_SOURCES += \
	atomicity.gcc.x86_64.cpp
endif

if MAKE_ATOMICITY_GCC_X86
libcxxtools_la_SOURCES += \
	atomicity.gcc.x86.cpp
endif

if MAKE_ATOMICITY_GCC_SPARC
libcxxtools_la_SOURCES += \
	atomicity.gcc.sparc.cpp
endif

if MAKE_ATOMICITY_GCC_AVR32
libcxxtools_la_SOURCES += \
	atomicity.gcc.avr32.cpp
endif

if MAKE_ATOMICITY_PTHREAD
libcxxtools_la_SOURCES += \
	atomicity.pthread.cpp
endif

if MAKE_ATOMICITY_GENERIC
libcxxtools_la_SOURCES += \
	atomicity.generic.cpp
endif

libcxxtools_la_LIBADD = $(LIBICONV)
libcxxtools_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libcxxtools_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

