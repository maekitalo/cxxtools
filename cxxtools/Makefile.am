ACLOCAL_AMFLAGS = -I m4

if MAKE_DEMOS
  DEMO_DIR = demo
endif

if MAKE_UNITTEST
  UNITTEST_DIR = test
endif

SUBDIRS = src \
    src/unit \
    src/http \
    src/xml \
    src/xmlrpc \
    include \
    $(DEMO_DIR) $(UNITTEST_DIR)

bin_SCRIPTS = cxxtools-config

EXTRA_dist = \
	m4/acx_pthread.m4 \
	m4/iconv.m4
