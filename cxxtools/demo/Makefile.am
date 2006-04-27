noinst_PROGRAMS = getini hd md5sum multifstream arg arg-set cgi dir \
  dlloader pool dynbuffer log logbench netcat httprequest netio \
  netmsg logsh thread

getini_SOURCES = getini.cpp
hd_SOURCES = hd.cpp
md5sum_SOURCES = md5sum.cpp
multifstream_SOURCES = multifstream.cpp
arg_SOURCES = arg.cpp
arg_set_SOURCES = arg-set.cpp
cgi_SOURCES = cgi.cpp
dir_SOURCES = dir.cpp
dlloader_SOURCES = dlloader.cpp
pool_SOURCES = pool.cpp
dynbuffer_SOURCES = dynbuffer.cpp
log_SOURCES = log.cpp
logbench_SOURCES = logbench.cpp
netcat_SOURCES = netcat.cpp
httprequest_SOURCES = httprequest.cpp
netio_SOURCES = netio.cpp
netmsg_SOURCES = netmsg.cpp
logsh_SOURCES = logsh.cpp
thread_SOURCES = thread.cpp

INCLUDES = -I$(top_builddir)/include
LDADD = $(top_builddir)/src/libcxxtools.la
AM_LDFLAGS = -L$(top_builddir)/src
log_LDFLAGS = @CXXTOOLS_LOG_LDFLAGS@
logbench_LDFLAGS = @CXXTOOLS_LOG_LDFLAGS@
logsh_LDFLAGS = @CXXTOOLS_LOG_LDFLAGS@

log.properties:
	$(top_builddir)/cxxtools-config --properties cxxtools >$@

data_DATA = log.properties
CLEANFILES = log.properties


