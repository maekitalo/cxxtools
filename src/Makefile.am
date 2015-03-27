AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include

lib_LTLIBRARIES = libcxxtools.la

libcxxtools_la_SOURCES = \
	addrinfo.cpp \
	addrinfoimpl.cpp \
	application.cpp \
	applicationimpl.cpp \
	base64codec.cpp \
	csvdeserializer.cpp \
	csvformatter.cpp \
	csvparser.cpp \
	char.cpp \
	clock.cpp \
	clockimpl.cpp \
	condition.cpp \
	conditionimpl.cpp \
	connectable.cpp \
	connection.cpp \
	cgi.cpp \
	conversionerror.cpp \
	convert.cpp \
	date.cpp \
	datetime.cpp \
	dateutils.cpp \
	decomposer.cpp \
	deserializer.cpp \
	directory.cpp \
	directoryimpl.cpp \
	error.cpp \
	eventloop.cpp \
	eventsink.cpp \
	eventsource.cpp \
	fdstream.cpp \
	file.cpp \
	filedevice.cpp \
	filedeviceimpl.cpp \
	fileimpl.cpp \
	fileinfo.cpp \
	formatter.cpp \
	hdstream.cpp \
	inifile.cpp \
	iniparser.cpp \
	iodevice.cpp \
	iodeviceimpl.cpp \
	ioerror.cpp \
	iostream.cpp \
	iso8859_1codec.cpp \
	iso8859_15codec.cpp \
	jsondeserializer.cpp \
	jsonformatter.cpp \
	jsonparser.cpp \
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
	posix/commandinput.cpp \
	posix/commandoutput.cpp \
	posix/daemonize.cpp \
	posix/pipestream.cpp \
	posix/posixpipe.cpp \
	propertiesfile.cpp \
	propertiesparser.cpp \
	propertiesdeserializer.cpp \
	query_params.cpp \
	quotedprintablestream.cpp \
	regex.cpp \
	remoteclient.cpp \
	selectable.cpp \
	selector.cpp \
	selectorimpl.cpp \
	semaphore.cpp \
	semaphoreimpl.cpp \
	serviceregistry.cpp \
	settings.cpp \
	settingsreader.cpp \
	settingswriter.cpp \
	serializationerror.cpp \
	serializationinfo.cpp \
	signal.cpp \
	streambuffer.cpp \
	string.cpp \
	stringstream.cpp \
	systemerror.cpp \
	tee.cpp \
	textbuffer.cpp \
	textcodec.cpp \
	textstream.cpp \
	thread.cpp \
	threadimpl.cpp \
	threadpool.cpp \
	threadpoolimpl.cpp \
	time.cpp \
	timer.cpp \
	timespan.cpp \
	uri.cpp \
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
	udpstream.cpp \
	xml/characters.cpp \
	xml/endelement.cpp \
	xml/entityresolver.cpp \
	xml/namespacecontext.cpp \
	xml/startelement.cpp \
	xml/xmldeserializer.cpp \
	xml/xmlerror.cpp \
	xml/xmlformatter.cpp \
	xml/xmlreader.cpp \
	xml/xmlserializer.cpp \
	xml/xmlwriter.cpp

noinst_HEADERS = \
	addrinfoimpl.h \
	applicationimpl.h \
	clockimpl.h \
	conditionimpl.h \
	dateutils.h \
	directoryimpl.h \
	error.h \
	facets.cpp \
	fileimpl.h \
	filedeviceimpl.h \
	fileinfoimpl.h \
	iodeviceimpl.h \
	libraryimpl.h \
	md5.h \
	muteximpl.h \
	pipeimpl.h \
	selectableimpl.h \
	selectorimpl.h \
	semaphoreimpl.h \
	settingsreader.h \
	settingswriter.h \
	threadimpl.h \
	threadpoolimpl.h \
	unicode.h \
	tcpserverimpl.h \
	tcpsocketimpl.h

if MAKE_ICONVSTREAM
libcxxtools_la_SOURCES += \
	iconvwrap.cpp \
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

if MAKE_ATOMICITY_GCC_SPARC32
libcxxtools_la_SOURCES += \
	atomicity.gcc.sparc32.cpp
endif

if MAKE_ATOMICITY_GCC_SPARC64
libcxxtools_la_SOURCES += \
	atomicity.gcc.sparc64.cpp
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
libcxxtools_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
