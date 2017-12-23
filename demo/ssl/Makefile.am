noinst_PROGRAMS = rpcbserver rpcjserver rpcJserver rpcxserver rpcbecho rpcjecho rpcJecho rpcxecho
rpcbserver_SOURCES = rpcbserver.cpp
rpcbserver_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/bin/libcxxtools-bin.la

rpcjserver_SOURCES = rpcjserver.cpp
rpcjserver_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/json/libcxxtools-json.la

rpcJserver_SOURCES = rpcJserver.cpp
rpcJserver_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/http/libcxxtools-http.la $(top_builddir)/src/json/libcxxtools-json.la

rpcxserver_SOURCES = rpcxserver.cpp
rpcxserver_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/http/libcxxtools-http.la $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la

rpcbecho_SOURCES = rpcbecho.cpp
rpcbecho_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/bin/libcxxtools-bin.la

rpcjecho_SOURCES = rpcjecho.cpp
rpcjecho_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/json/libcxxtools-json.la

rpcJecho_SOURCES = rpcJecho.cpp
rpcJecho_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/http/libcxxtools-http.la $(top_builddir)/src/json/libcxxtools-json.la

rpcxecho_SOURCES = rpcxecho.cpp
rpcxecho_LDADD=$(top_builddir)/src/libcxxtools.la $(top_builddir)/src/xmlrpc/libcxxtools-xmlrpc.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/include -I$(top_srcdir)/include
