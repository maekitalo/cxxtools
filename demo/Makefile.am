noinst_HEADERS = \
	msg.h

noinst_PROGRAMS = \
	arg \
	arg-set \
	commandoutput \
	cxxlog \
	deserialization \
	dir \
	dlloader \
	execLs \
	getini hd \
	httpclient \
	httprequest \
	httpserver \
	insrv \
	json \
	jsonbeautify \
	log \
	logsh \
	md5sum \
	mime \
	mime2json \
	mimeextract \
	mimemultipart \
	msgclient \
	msgserver \
	multifstream \
	netcat \
	netio \
	netmsg \
	pipestream \
	pool \
	regex \
	rpcaddclient \
	rpcasyncaddclient \
	rpcechoclient \
	rpcparallelecho \
	rpcserver \
	saml2 \
	serialization \
	signals \
	splitter \
	sslshowcert \
	tcpsocket \
	timer \
	timer-lambda \
	uuencode \
	xmlmsgclient \
	xmlmsgserver \
	xmlreader \
	dynamicrpc \
	dynamiccall \
	rpcproxy

arg_set_SOURCES = arg-set.cpp
arg_SOURCES = arg.cpp
commandoutput_SOURCES = commandoutput.cpp
cxxlog_SOURCES = cxxlog.cpp
deserialization_SOURCES = deserialization.cpp
dir_SOURCES = dir.cpp
dlloader_SOURCES = dlloader.cpp
execLs_SOURCES = execLs.cpp
getini_SOURCES = getini.cpp
hd_SOURCES = hd.cpp
httpclient_SOURCES = httpclient.cpp
httprequest_SOURCES = httprequest.cpp
httpserver_SOURCES = httpserver.cpp
insrv_SOURCES = insrv.cpp
jsonbeautify_SOURCES = jsonbeautify.cpp
json_SOURCES = json.cpp
logsh_SOURCES = logsh.cpp
log_SOURCES = log.cpp
md5sum_SOURCES = md5sum.cpp
mime2json_SOURCES = mime2json.cpp
mimeextract_SOURCES = mimeextract.cpp
mimemultipart_SOURCES = mimemultipart.cpp
mime_SOURCES = mime.cpp
msgclient_SOURCES = msgclient.cpp
msgserver_SOURCES = msgserver.cpp
multifstream_SOURCES = multifstream.cpp
netcat_SOURCES = netcat.cpp
netio_SOURCES = netio.cpp
netmsg_SOURCES = netmsg.cpp
pipestream_SOURCES = pipestream.cpp
pool_SOURCES = pool.cpp
regex_SOURCES = regex.cpp
rpcaddclient_SOURCES = rpcaddclient.cpp
rpcasyncaddclient_SOURCES = rpcasyncaddclient.cpp
rpcechoclient_SOURCES = rpcechoclient.cpp
rpcparallelecho_SOURCES = rpcparallelecho.cpp
rpcserver_SOURCES = rpcserver.cpp
saml2_SOURCES = saml2.cpp
serialization_SOURCES = serialization.cpp
signals_SOURCES = signals.cpp
splitter_SOURCES = splitter.cpp
sslshowcert_SOURCES = sslshowcert.cpp
tcpsocket_SOURCES = tcpsocket.cpp
timer_SOURCES = timer.cpp
timer_lambda_SOURCES = timer-lambda.cpp
uuencode_SOURCES = uuencode.cpp
xmlmsgclient_SOURCES = xmlmsgclient.cpp
xmlmsgserver_SOURCES = xmlmsgserver.cpp
xmlreader_SOURCES = xmlreader.cpp
dynamicrpc_SOURCES = dynamicrpc.cpp
dynamiccall_SOURCES = dynamiccall.cpp
rpcproxy_SOURCES = rpcproxy.cpp

BASE_LIBS = $(top_builddir)/src/libcxxtools.la
HTTP_LIBS = $(BASE_LIBS) $(top_builddir)/src/http/libcxxtools-http.la
XMLRPC_LIBS = $(HTTP_LIBS) $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
BIN_LIBS = $(BASE_LIBS) $(top_builddir)/src/bin/libcxxtools-bin.la
JSON_LIBS = $(HTTP_LIBS) $(top_builddir)/src/json/libcxxtools-json.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include
LDADD = $(top_builddir)/src/libcxxtools.la

httpclient_LDADD =  $(HTTP_LIBS)
httprequest_LDADD = $(HTTP_LIBS)
httpserver_LDADD =  $(HTTP_LIBS)
rpcserver_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
rpcechoclient_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
rpcaddclient_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
json_LDADD = $(HTTP_LIBS)
jsonbeautify_LDADD = $(HTTP_LIBS)
rpcasyncaddclient_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
rpcparallelecho_LDADD = $(BIN_LIBS)
msgclient_LDADD = $(BIN_LIBS)
msgserver_LDADD = $(BIN_LIBS)
xmlmsgclient_LDADD = $(XMLRPC_LIBS)
xmlmsgserver_LDADD = $(XMLRPC_LIBS)
dynamicrpc_LDADD = $(BIN_LIBS)
dynamiccall_LDADD = $(BIN_LIBS)
rpcproxy_LDADD = $(BIN_LIBS)

EXTRA_DIST = README
