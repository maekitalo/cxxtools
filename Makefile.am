ACLOCAL_AMFLAGS = -I m4

if MAKE_DEMOS
  DEMO_DIR = demo demo/async demo/ssl
endif

if MAKE_UNITTEST
  UNITTEST_DIR = test
endif

SUBDIRS = src \
    src/unit \
    src/http \
    src/xmlrpc \
    src/bin \
    src/json \
    include \
    tools \
    doc \
    $(DEMO_DIR) $(UNITTEST_DIR)

pkgconfigdir = $(libdir)/pkgconfig/

pkgconfig_DATA = pkgconfig/cxxtools-bin.pc \
	pkgconfig/cxxtools-http.pc \
	pkgconfig/cxxtools-json.pc \
	pkgconfig/cxxtools.pc \
	pkgconfig/cxxtools-unit.pc \
	pkgconfig/cxxtools-xmlrpc.pc

EXTRA_DIST = README.md
