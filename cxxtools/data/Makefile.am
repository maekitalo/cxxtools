pkgdata_DATA = cxxtools-cxxflags cxxtools-ldflags cxxtools-log-ldflags

cxxtools-cxxflags: $(top_builddir)/src/stamp-h1
	$(ECHO) "-I$(includedir)" > $@

cxxtools-ldflags: $(top_builddir)/src/stamp-h1
	$(ECHO) "-L$(libdir) -lcxxtools" > $@

cxxtools-log-ldflags: $(top_builddir)/src/stamp-h1
	$(ECHO) "-L$(libdir) @logger_ldflags@ " > $@
