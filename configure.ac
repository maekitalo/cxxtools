AC_INIT(cxxtools, 3.0, [Tommi Maekitalo <tommi@tntnet.org>])
AM_INIT_AUTOMAKE
AC_USE_SYSTEM_EXTENSIONS

LT_INIT([disable-static])

AC_PREREQ([2.5.9])

abi_current=10
abi_revision=0
abi_age=0
sonumber=${abi_current}:${abi_revision}:${abi_age}
AC_SUBST(sonumber)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/config.h ])
AC_CONFIG_FILES([include/cxxtools/config.h])
AC_CONFIG_SRCDIR([src/log.cpp])

AC_HEADER_DIRENT

AC_PROG_CC
AC_PROG_CXX

AC_LANG(C++)
AX_CXX_COMPILE_STDCXX_11(noext, mandatory)

AX_COMPILER_VENDOR
AS_IF([test "$ax_cv_cxx_compiler_vendor" = "ibm"],
      [CPPFLAGS="$CPPFLAGS -qrtti -qlanglvl=newexcp -D__NOLOCK_ON_INPUT -D__NOLOCK_ON_OUTPUT"],
      AX_CHECK_COMPILE_FLAG([-Wno-long-long], [CPPFLAGS="$CPPFLAGS -Wno-long-long"])
      AX_CHECK_COMPILE_FLAG([-Wall], [CPPFLAGS="$CPPFLAGS -Wall"])
      AX_CHECK_COMPILE_FLAG([-pedantic], [CPPFLAGS="$CPPFLAGS -pedantic"]))

AC_CHECK_HEADERS(sys/filio.h)
AC_CHECK_HEADERS(csignal)
AC_CHECK_HEADERS([sys/sendfile.h])

AC_CHECK_LIB(nsl, setsockopt)
AC_CHECK_LIB(socket, accept)
AC_CHECK_LIB(rt, sem_destroy)
AC_SEARCH_LIBS(dlopen, dl, , AC_MSG_ERROR([dlopen not found]))
AC_SEARCH_LIBS(inet_ntop, nsl socket resolv)
AC_SEARCH_LIBS(clock_gettime, rt)
AC_CHECK_FUNCS(inet_ntop accept4)
AC_CHECK_FUNCS(clock_gettime)
AC_CHECK_FUNCS(nanosleep)
AC_CHECK_FUNCS(sendfile)
AC_CHECK_FUNCS(ppoll)
AC_TYPE_LONG_LONG_INT
AC_TYPE_UNSIGNED_LONG_LONG_INT

AS_IF([test "$ac_cv_type_long_long_int" = yes],
[
    HAVE_LONG_LONG=HAVE_LONG_LONG

    AC_COMPILE_IFELSE(
      [AC_LANG_SOURCE([
        #include <stdint.h>
        void f(short);
        void f(int);
        void f(long);
        void f(long long);
        void f() { int64_t v = 1; f(v); }
        ])],
      [AC_DEFINE(INT64_IS_BASETYPE, 1, [defined if int64_t is one of the base types])])
], [
    AC_MSG_WARN([long long not found]);
    HAVE_LONG_LONG=NO_LONG_LONG

    AC_COMPILE_IFELSE(
      [AC_LANG_SOURCE([
        #include <stdint.h>
        void f(short);
        void f(int);
        void f(long);
        void f() { int64_t v = 1; f(v); }
        ])],
      [AC_DEFINE(INT64_IS_BASETYPE, 1, [defined if int64_t is one of the base types])])
])

AC_SUBST(HAVE_LONG_LONG, "$HAVE_LONG_LONG")

AS_IF([test "$ac_cv_type_unsigned_long_long_int" = yes],
[
    HAVE_UNSIGNED_LONG_LONG=HAVE_UNSIGNED_LONG_LONG
], [
    AC_MSG_WARN([unsigned long long not found ($ac_cv_type_unsigned_long_long_int)]);
    HAVE_UNSIGNED_LONG_LONG=NO_UNSIGNED_LONG_LONG
])

AC_SUBST(HAVE_UNSIGNED_LONG_LONG, "$HAVE_UNSIGNED_LONG_LONG")

AC_ARG_WITH([iconvstream],
    [AS_HELP_STRING([--with-iconvstream=yes|no], [compile iconv stream (default: no)])],
    [with_iconvstream=$withval],
    [with_iconvstream=no])

AS_IF([test "$with_iconvstream" = yes],
[
  AM_ICONV
  AS_IF([test "$am_cv_func_iconv" !=  yes],
    AC_MSG_ERROR(iconv not found))
])

AM_CONDITIONAL(MAKE_ICONVSTREAM, test $with_iconvstream = yes)

ACX_PTHREAD

CC="$PTHREAD_CC"
CXX="$PTHREAD_CXX"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CXXFLAGS"
LIBS="$LIBS $PTHREAD_LIBS"

CXXTOOLS_CXXFLAGS='-I${includedir}'
CXXTOOLS_LDFLAGS='-L${libdir} -lcxxtools'

AC_SUBST(CXXTOOLS_CXXFLAGS)
AC_SUBST(CXXTOOLS_LDFLAGS)

AS_CASE(["${host_cpu}-${host_os}"],
    [*-aix*], [SHARED_LIB_FLAG=-qmkshrobj],
    [*-darwin*], [SHARED_LIB_FLAG=-dynamiclib],
    [SHARED_LIB_FLAG=-shared]
)
AC_SUBST(SHARED_LIB_FLAG)

#
# checking existence of suseconds_t, needed by hirestime.h
#
AC_CHECKING(for suseconds_t)

AC_CHECK_TYPE(
  suseconds_t,
  [CXXTOOLS_SUSECONDS=CXXTOOLS_SUSECONDS_T],
  [CXXTOOLS_SUSECONDS=CXXTOOLS_SUSECONDS_TIME_T],
  [#include <sys/time.h>])

AC_SUBST(CXXTOOLS_SUSECONDS)

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([#include <locale>
    void foo() { std::numpunct<char>* np; }
  ])],
  [CXXTOOLS_STD_LOCALE=CXXTOOLS_WITH_STD_LOCALE],
  [CXXTOOLS_STD_LOCALE=CXXTOOLS_WITHOUT_STD_LOCALE])

AC_SUBST(CXXTOOLS_STD_LOCALE)

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([#include <netinet/tcp.h>
   int i = TCP_DEFER_ACCEPT;
  ])],
  AC_DEFINE(HAVE_TCP_DEFER_ACCEPT, 1, [defined if TCP_DEFER_ACCEPT is defined]))

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([#include <sys/socket.h>
   #include <netinet/in.h>
   int i = IPPROTO_IPV6;
  ])],
  AC_DEFINE(HAVE_IPV6, 1, [defined if IPV6 is supported]))

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
   #include <sys/types.h>
   #include <sys/socket.h>
   int i = SO_NOSIGPIPE;
  ])],
  AC_DEFINE(HAVE_SO_NOSIGPIPE, 1, [defined if socket option SO_NOSIGPIPE is supported]))

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
   #include <sys/types.h>
   #include <sys/socket.h>
   int i = MSG_NOSIGNAL;
  ])],
  AC_DEFINE(HAVE_MSG_NOSIGNAL, 1, [defined if MSG_NOSIGNAL is defined]))

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([#include <iterator>
   std::reverse_iterator<char*> r;])],
  [HAVE_REVERSE_ITERATOR=HAVE_REVERSE_ITERATOR],
  [HAVE_REVERSE_ITERATOR=NO_REVERSE_ITERATOR])

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([#include <iterator>
   std::reverse_iterator<char*, int, char, char*, char&> r;])],
  [HAVE_REVERSE_ITERATOR_4=HAVE_REVERSE_ITERATOR_4],
  [HAVE_REVERSE_ITERATOR_4=NO_REVERSE_ITERATOR_4])

AC_SUBST(HAVE_REVERSE_ITERATOR, "$HAVE_REVERSE_ITERATOR")
AC_SUBST(HAVE_REVERSE_ITERATOR_4, "$HAVE_REVERSE_ITERATOR_4")

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
   #include <cxxabi.h>
   void foo() { try {} catch (const abi::__forced_unwind&) {} }
  ])],
  [AC_DEFINE(HAVE_ABI_FORCED_UNWIND, 1, [defined if exception abi::__forced_unwind exists])])

#
# SSL
#
AC_ARG_WITH([ssl],
    [AS_HELP_STRING([--with-ssl=yes|no], [build with SSL support])],
    [ssl_option=$withval],
    [ssl_option=yes])

AS_IF([test $ssl_option = yes],
[
  AC_CHECK_HEADER([openssl/ssl.h],
      [
        AC_MSG_RESULT([building with ssl support])
        AC_DEFINE(WITH_SSL, [], [Define to build with openssl])
        AC_COMPILE_IFELSE(
            [AC_LANG_SOURCE([
                #include <openssl/asn1.h>
                ASN1_TIME* t;
                void foo() { ASN1_TIME_diff(0, 0, t, t); }
            ])],
            [AC_DEFINE(HAVE_ASN1_TIME_diff, [], [Defined when openssl supports ASN1_TIME_diff (openssl 1.0.2)])],
            []
        )
      ],
      AC_MSG_ERROR([header for openssl not found; install openssl developent package or use --without-ssl])
      )
])

AM_CONDITIONAL(MAKE_OPENSSL, test $ssl_option = yes)

AC_ARG_ENABLE([demos],
  [AS_HELP_STRING([--disable-demos], [disable building demos])],
  [enable_demos=$enableval],
  [enable_demos=enable_demos])

AM_CONDITIONAL(MAKE_DEMOS, test "$enable_demos" = "enable_demos")

AC_ARG_ENABLE([unittest],
  [AS_HELP_STRING([--disable-unittest], [disable building unittest])],
  [enable_unittest=$enableval],
  [enable_unittest=enable_unittest])

AM_CONDITIONAL(MAKE_UNITTEST, test "$enable_unittest" = "enable_unittest")

AC_CONFIG_FILES([cxxtools-config], [chmod +x cxxtools-config])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/unit/Makefile
  src/http/Makefile
  src/xmlrpc/Makefile
  src/bin/Makefile
  src/json/Makefile
  include/Makefile
  tools/Makefile
  demo/Makefile
  demo/async/Makefile
  demo/ssl/Makefile
  test/Makefile
  ])

AC_CONFIG_FILES([
  pkgconfig/cxxtools-bin.pc 
  pkgconfig/cxxtools-http.pc 
  pkgconfig/cxxtools-json.pc 
  pkgconfig/cxxtools.pc
  pkgconfig/cxxtools-unit.pc
  pkgconfig/cxxtools-xmlrpc.pc
])

AC_OUTPUT


