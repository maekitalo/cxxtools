noinst_PROGRAMS = getini hdstream md5sum multifstream arg arg-set cgi dir \
  dlloader pool dynbuffer log logbench netcat netecho httprequest

getini_SOURCES = getini.cpp
hdstream_SOURCES = hdstream.cpp
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
netecho_SOURCES = netecho.cpp
httprequest_SOURCES = httprequest.cpp

EXTRA_DIST = log.properties \
    log4j.properties \
    log4cplus.properties

INCLUDES = -I$(top_builddir)/include
LDADD = $(top_builddir)/src/libcxxtools.la
AM_LDFLAGS = -L$(top_builddir)/src
log_LDADD = @logger_ldflags@
logbench_LDADD = @logger_ldflags@
