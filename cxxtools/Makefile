DESTDIR = $(ROOTDIR)/usr/local
LIBDIR = $(DESTDIR)/lib
INCDIR = $(DESTDIR)/include

export LIBDIR
export INCLUDEDIR

.PHONY: lib debian test

all: lib

lib:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(INCDIR):
	mkdir -p $(INCDIR)

test:
	$(MAKE) -C test

install: $(LIBDIR) $(INCDIR)
	$(MAKE) -C src install
	$(MAKE) -C include install

debian:
	dpkg-buildpackage -rfakeroot -b -uc

