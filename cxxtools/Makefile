INSTALL_DIR=/usr/local/cxxtools
PROD_LIB_DIR=$(INSTALL_DIR)/lib
SDK_PUBLIC_INC_DIR=$(INSTALL_DIR)/include

export VERSION=1.0.1

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
	@if [ ! -d $(SDK_PUBLIC_INC_DIR)/cxxtools ]; \
	then                                         \
	  mkdir -p $(SDK_PUBLIC_INC_DIR)/cxxtools;  \
	fi
	@rm -rf $(PROD_PUBLIC_INC_DIR)/cxxtools/*
	cp -p include/cxxtools/*.h $(SDK_PUBLIC_INC_DIR)/cxxtools/
	@if [ ! -d $(INSTALL_DIR)/make ];  \
	then                          \
	  mkdir -p $(INSTALL_DIR)/make;   \
	fi
	cp -p make/cxxtools.mk $(INSTALL_DIR)/make/

