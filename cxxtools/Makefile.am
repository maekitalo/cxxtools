ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src include demo
if MAKE_LIBTOOL
  SUBDIRS+=libltdl
endif

bin_SCRIPTS = cxxtools-config

EXTRA_dist = \
	m4/acx_pthread.m4 \
	m4/iconv.m4
