ifndef DESTDIR
	export DESTDIR=/
endif

ifndef LIBDIR
	LIBDIR=$(DESTDIR)/usr/lib
endif

ifndef INCLUDEDIR
	INCLUDEDIR=$(DESTDIR)/usr/include
endif
export LIBDIR
export INCLUDEDIR

.PHONY: lib debian test

all: lib

lib:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

$(DESTDIR)/usr/lib:
	mkdir -p $(DESTDIR)/usr/lib

$(DESTDIR)/usr/include:
	mkdir -p $(DESTDIR)/usr/include

test:
	$(MAKE) -C test

install: $(DESTDIR)/usr/lib $(DESTDIR)/usr/include
	$(MAKE) -C src install
	$(MAKE) -C include install

debian:
	dpkg-buildpackage -rfakeroot -b -uc

