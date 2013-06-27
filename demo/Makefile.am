noinst_PROGRAMS = arg arg-set cgi dir dlloader getini hd \
	httprequest httpserver log logbench logsh md5sum mime multifstream netcat \
	netio netmsg pipestream pool signals thread threadpool uuencode cxxlog \
	rpcserver rpcechoclient rpcaddclient splitter json regex execLs rpcasyncaddclient \
	deserialization serialization

arg_SOURCES = arg.cpp
arg_set_SOURCES = arg-set.cpp
cgi_SOURCES = cgi.cpp
dir_SOURCES = dir.cpp
dlloader_SOURCES = dlloader.cpp
getini_SOURCES = getini.cpp
hd_SOURCES = hd.cpp
httprequest_SOURCES = httprequest.cpp
httpserver_SOURCES = httpserver.cpp
log_SOURCES = log.cpp
logbench_SOURCES = logbench.cpp
logsh_SOURCES = logsh.cpp
md5sum_SOURCES = md5sum.cpp
mime_SOURCES = mime.cpp
multifstream_SOURCES = multifstream.cpp
netcat_SOURCES = netcat.cpp
netio_SOURCES = netio.cpp
netmsg_SOURCES = netmsg.cpp
pipestream_SOURCES = pipestream.cpp
pool_SOURCES = pool.cpp
signals_SOURCES = signals.cpp
thread_SOURCES = thread.cpp
threadpool_SOURCES = threadpool.cpp
uuencode_SOURCES = uuencode.cpp
cxxlog_SOURCES = cxxlog.cpp
rpcserver_SOURCES = rpcserver.cpp
rpcechoclient_SOURCES = rpcechoclient.cpp
rpcaddclient_SOURCES = rpcaddclient.cpp
deserialization_SOURCES = deserialization.cpp
serialization_SOURCES = serialization.cpp
splitter_SOURCES = splitter.cpp
json_SOURCES = json.cpp
regex_SOURCES = regex.cpp
execLs_SOURCES = execLs.cpp
rpcasyncaddclient_SOURCES = rpcasyncaddclient.cpp

if MAKE_ICONVSTREAM
noinst_PROGRAMS += iconv
iconv_SOURCES = iconv.cpp
endif

BASE_LIBS = $(top_builddir)/src/libcxxtools.la
HTTP_LIBS = $(BASE_LIBS) $(top_builddir)/src/http/libcxxtools-http.la
XMLRPC_LIBS = $(BASE_LIBS) $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la
BIN_LIBS = $(HTTP_LIBS) $(top_builddir)/src/bin/libcxxtools-bin.la
JSON_LIBS = $(HTTP_LIBS) $(top_builddir)/src/json/libcxxtools-json.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include
LDADD = $(top_builddir)/src/libcxxtools.la

httpserver_LDADD =  $(HTTP_LIBS)
httprequest_LDADD = $(HTTP_LIBS)
rpcserver_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
rpcechoclient_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
rpcaddclient_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)
json_LDADD = $(HTTP_LIBS)
rpcasyncaddclient_LDADD = $(XMLRPC_LIBS) $(BIN_LIBS) $(JSON_LIBS)

log.xml:
	/bin/sh $(top_builddir)/cxxtools-config --logxml cxxtools >$@

noinst_DATA = log.xml
CLEANFILES = log.xml
EXTRA_DIST = README
