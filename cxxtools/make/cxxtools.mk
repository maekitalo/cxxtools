ifndef CXXTOOLS_MK__
CXXTOOLS_MK__=1

#
# CXXTOOLS additional settings
#

ifeq ($(CXXTOOLS), )
  CXXTOOLS=CXXTOOLS_1_0_1
endif

ifeq ($(CXXTOOLS), CXXTOOLS_1_0_1)
  CXXTOOLS_INSTALL_DIR=/usr/local/cxxtools
  CXXTOOLS_LIB_DIR=$(CXXTOOLS_INSTALL_DIR)/lib
  CCFLAGS +=-I$(CXXTOOLS_INSTALL_DIR)/include
  LDFLAGS +=-L$(CXXTOOLS_LIB_DIR) -lcxxtools
endif

endif # CXXTOOLS_MK__

