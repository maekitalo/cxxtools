INCLUDES= -I$(top_builddir)/include

lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	addrinfo.cpp \
	application.cpp \
	applicationimpl.cpp \
	base64stream.cpp \
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
	directory.cpp \
	directoryimpl.cpp \
	eventloop.cpp \
	eventsink.cpp \
	eventsource.cpp \
	fdstream.cpp \
	file.cpp \
	fileimpl.cpp \
	fileinfo.cpp \
	hdstream.cpp \
	httpclient.cpp \
	httpmessageheader.cpp \
	httpparser.cpp \
	httpserver.cpp \
	httpsocket.cpp \
	httpresponder.cpp \
	iconvstream.cpp \
	inifile.cpp \
	iniparser.cpp \
	iodevice.cpp \
	iodeviceimpl.cpp \
	ioerror.cpp \
	iostream.cpp \
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
	pipeimpl.cpp \
	pipestream.cpp \
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
	signal.cpp \
	streambuffer.cpp \
	systemerror.cpp \
	tcpserverimpl.cpp \
	tcpserver.cpp \
	tcpsocket.cpp \
	tcpsocketimpl.cpp \
	tcpstream.cpp \
	tee.cpp \
	textcodec.cpp \
	textstream.cpp \
	thread.cpp \
	threadimpl.cpp \
	time.cpp \
	timer.cpp \
	timespan.cpp \
	udp.cpp \
	udpstream.cpp \
	uuencode.cpp \
	xmltag.cpp

noinst_HEADERS = \
	addrinfo.h \
	applicationimpl.h \
	clockimpl.h \
	conditionimpl.h \
	directoryimpl.h \
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
	tcpserverimpl.h \
	tcpsocketimpl.h \
	threadimpl.h \
	unicode.h

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

