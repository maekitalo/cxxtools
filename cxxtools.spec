Name:		cxxtools
Summary:	some useful C++-classes
Version:	1.4.4
Release:	1
License:	GPL
Group:		Development/Languages/C and C++
Url:            http://www.tntnet.org
Source:		http://www.tntnet.org/download/cxxtools-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-build

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
dynbuffer - dynamic buffer

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
./configure --prefix=%{_prefix} --libdir=%{_libdir} --sysconfdir=/etc  --disable-final
make


%install
make MANDIR=%{buildroot}%{_mandir} \
     BINDIR=%{buildroot}%{_sbindir} \
     DESTDIR=$RPM_BUILD_ROOT \
     install
rm %{buildroot}%{_libdir}/libcxxtools.la

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/lib*.so*

%files devel
%{_libdir}/lib*.a
%{_includedir}/cxxtools/*.h
%{_includedir}/cxxtools/log/*.h
%{_bindir}/cxxtools-config

%changelog
