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
    src/bin \
    src/json \
    include \
    $(DEMO_DIR) $(UNITTEST_DIR)

bin_SCRIPTS = cxxtools-config
