Summary: some useful C++-classes
Name: cxxtools
Version: 1.4.1pre1
Release: 2
License: GPL
Group: Development/Languages/C and C++
Source: cxxtools-1.4.1pre1.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
some useful C++-casses:
arg - template-class for argc/argv-parsing
tcp::Stream - socket-library with std::iostream-interface
dlloader - C++-interface for dlopen/dlsym
query_params - CGI-parameter-class with parser
thread - C++-pthread-wrapper
tee - doubles a ostream-output
hdstream - ostream-hexdumper
iconvstream - ostream-interface to iconv(3) (codeset conversion)
md5stream - ostream for md5-calculation
dynbuffer - thread-safe dynamic buffer

Authors:
--------
    Tommi Maekitalo <tommi@tntnet.org>

%package devel
Summary: Include Files and Libraries mandatory for Development.
Group: Development/Languages/C and C++
Requires:     cxxtools = %{version}

%description devel
This package contains all necessary include files and libraries needed
to develop applications that require the provided includes and
libraries.

Authors:
--------
    Tommi Maekitalo <tommi@tntnet.org>

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" \
./configure --prefix=%{_prefix} --libdir=%{_libdir}
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/%{_lib}/lib*.so.*
/usr/%{_lib}/libcxxtools.la

%files devel
/usr/%{_lib}/lib*.so
/usr/%{_lib}/lib*.a
/usr/include/cxxtools/*.h
/usr/include/cxxtools/log/*.h
/usr/bin/cxxtools-config

%changelog
