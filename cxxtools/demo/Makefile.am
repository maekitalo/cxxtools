noinst_PROGRAMS = getini hdstream md5sum multifstream arg arg2 cgi dir \
  dlloader pool dynbuffer log logbench

getini_SOURCES = getini.cpp
hdstream_SOURCES = hdstream.cpp
md5sum_SOURCES = md5sum.cpp
multifstream_SOURCES = multifstream.cpp
arg_SOURCES = arg.cpp
arg2_SOURCES = arg2.cpp
cgi_SOURCES = cgi.cpp
dir_SOURCES = dir.cpp
dlloader_SOURCES = dlloader.cpp
pool_SOURCES = pool.cpp
dynbuffer_SOURCES = dynbuffer.cpp
log_SOURCES = log.cpp
logbench_SOURCES = logbench.cpp

EXTRA_DIST = log.properties \
    log4j.properties \
    log4cplus.properties

INCLUDES = -I../include
LDADD = $(top_builddir)/src/libcxxtools.la
AM_LDFLAGS = -L$(top_builddir)/src
log_LDADD = @logger_ldflags@
logbench_LDADD = @logger_ldflags@
