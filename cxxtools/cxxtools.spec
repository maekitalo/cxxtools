Summary: some useful C++-classes
Name: cxxtools
Version: 1.0.0
Release: 1
License: GPL
Group: Development/Languages/C and C++
Source: cxxtools-1.0.0.tgz
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
pollclass - class-interface for poll(2)

%package devel
Summary: Include Files and Libraries mandatory for Development.
Group: Development/Languages/C and C++
Requires:     cxxtools = %{version}

%description devel
This package contains all necessary include files and libraries needed
to develop applications that require the provided includes and
libraries.

%prep
%setup

%build
make

%install
make DESTDIR=$RPM_BUILD_ROOT/usr LIBDIR=$RPM_BUILD_ROOT/usr/%{_lib} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README
/usr/%{_lib}/lib*.so*

%files devel
/usr/include/cxxtools/*.h

%changelog
