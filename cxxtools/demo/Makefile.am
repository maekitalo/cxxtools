noinst_PROGRAMS = getini hdstream md5sum multifstream arg cgi dir \
  dlloader pool dynbuffer log

getini_SOURCES = getini.cpp
hdstream_SOURCES = hdstream.cpp
md5sum_SOURCES = md5sum.cpp
multifstream_SOURCES = multifstream.cpp
arg_SOURCES = arg.cpp
cgi_SOURCES = cgi.cpp
dir_SOURCES = dir.cpp
dlloader_SOURCES = dlloader.cpp
pool_SOURCES = pool.cpp
dynbuffer_SOURCES = dynbuffer.cpp
log_SOURCES = log.cpp

EXTRA_DIST = run_log

INCLUDES = -I../include
LDADD = $(top_builddir)/src/libcxxtools.la
log_LDADD = $(top_builddir)/src/libcxxtools-log.la
