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
	charmapcodec.cpp \
	clock.cpp \
	clockimpl.cpp \
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
	envsubst.cpp \
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
	inideserializer.cpp \
	inifile.cpp \
	iniparser.cpp \
	iniserializer.cpp \
	iodevice.cpp \
	iodeviceimpl.cpp \
	ioerror.cpp \
	iostream.cpp \
	iso8859_codec.cpp \
	jsondeserializer.cpp \
	jsonformatter.cpp \
	jsonparser.cpp \
	library.cpp \
	libraryimpl.cpp \
	log.cpp \
	md5.c \
	md5stream.cpp \
	mime.cpp \
	multifstream.cpp \
	pipe.cpp \
	pipeimpl.cpp \
	posix/commandinput.cpp \
	posix/commandoutput.cpp \
	posix/daemonize.cpp \
	posix/fork.cpp \
	posix/pipestream.cpp \
	posix/posixpipe.cpp \
	propertiesfile.cpp \
	propertiesparser.cpp \
	propertiesdeserializer.cpp \
	propertiesserializer.cpp \
	query_params.cpp \
	quotedprintablecodec.cpp \
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
	sslcertificate.cpp \
	stddevice.cpp \
	streambuffer.cpp \
	string.cpp \
	stringstream.cpp \
	systemerror.cpp \
	tee.cpp \
	textbuffer.cpp \
	textcodec.cpp \
	textstream.cpp \
	time.cpp \
	timer.cpp \
	timespan.cpp \
	uri.cpp \
	utf8codec.cpp \
	uuencode.cpp \
	net.cpp \
	tcpserverimpl.cpp \
	tcpserver.cpp \
	tcpsocket.cpp \
	tcpsocketimpl.cpp \
	tcpstream.cpp \
	tz.cpp \
	udp.cpp \
	udpstream.cpp \
	win1252codec.cpp \
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
	dateutils.h \
	directoryimpl.h \
	error.h \
	facets.cpp \
	fileimpl.h \
	filedeviceimpl.h \
	iodeviceimpl.h \
	libraryimpl.h \
	md5.h \
	pipeimpl.h \
	selectableimpl.h \
	selectorimpl.h \
	semaphoreimpl.h \
	settingsreader.h \
	settingswriter.h \
	sslcertificateimpl.h \
	sslctximpl.h \
	tcpserverimpl.h \
	tcpsocketimpl.h \
	unicode.h

if MAKE_ICONVSTREAM
libcxxtools_la_SOURCES += \
	iconvwrap.cpp \
	iconvstream.cpp
endif

libcxxtools_la_LIBADD = $(LIBICONV)
libcxxtools_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@

if MAKE_OPENSSL
libcxxtools_la_LDFLAGS += -lssl
libcxxtools_la_SOURCES += \
	sslcertificateimpl.cpp \
	sslctx.cpp \
	sslctximpl.cpp
endif
