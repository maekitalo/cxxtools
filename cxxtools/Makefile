ifndef INSTALL_ROOT_DIR
  INSTALL_ROOT_DIR=/usr/local
endif
export INSTALL_ROOT_DIR

ifndef INSTALL_DIRECTORY
  INSTALL_DIRECTORY=$(INSTALL_ROOT_DIR)/cxxtools
endif
export INSTALL_DIRECTORY

PROD_LIB_DIR=$(INSTALL_DIRECTORY)/lib
PROD_MAKE_DIR=$(INSTALL_DIRECTORY)/make
PROD_INC_DIR=$(INSTALL_DIRECTORY)/include

export VERSION=1.0.1

.PHONY: all depend clean install src Makefile

all:
	make -C src all

depend:
	make -C src depend

clean:
	make -C src clean

install: all
	@if [ ! -d $(PROD_LIB_DIR) ];  \
	then                          \
	  mkdir -p $(PROD_LIB_DIR);   \
	fi
	@rm -f $(PROD_LIB_DIR)/libcxxtools.so*
	cp -p src/libcxxtools.so.$(VERSION) $(PROD_LIB_DIR)
	ln -s $(PROD_LIB_DIR)/libcxxtools.so.$(VERSION) $(PROD_LIB_DIR)/libcxxtools.so
	@if [ ! -d $(PROD_INC_DIR)/cxxtools ]; \
	then                                         \
	  mkdir -p $(PROD_INC_DIR)/cxxtools;  \
	fi
	@rm -rf $(PRODC_INC_DIR)/cxxtools/*
	cp -p include/cxxtools/*.h $(PROD_INC_DIR)/cxxtools/
	@if [ ! -d $(PROD_MAKE_DIR) ];  \
	then                          \
	  mkdir -p $(PROD_MAKE_DIR);   \
	fi
	cp -p make/cxxtools.mk $(PROD_MAKE_DIR)/

