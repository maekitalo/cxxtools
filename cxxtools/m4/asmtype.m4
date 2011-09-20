dnl Copyright (C) 2008 by Tommi Maekitalo
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl 
dnl As a special exception, you may use this file as part of a free
dnl software library without restriction. Specifically, if other files
dnl instantiate templates or use macros or inline functions from this
dnl file, or you compile this file and link it with other files to
dnl produce an executable, this file does not by itself cause the
dnl resulting executable to be covered by the GNU General Public
dnl License. This exception does not however invalidate any other
dnl reasons why the executable file might be covered by the GNU Library
dnl General Public License.
dnl 
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl 
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
dnl
dnl
AC_DEFUN([AC_CHECKATOMICTYPE],
  [
    if test "$ac_cxxtools_atomicity" = "$1" -o "$ac_cxxtools_atomicity" = "probe"
    then
      AC_COMPILE_IFELSE(
        [AC_LANG_SOURCE([$3])],
        CXXTOOLS_ATOMICITY=$2
        ac_cxxtools_atomicity=$1,
        if test "$ac_cxxtools_atomicity" = "$1"
        then
          AC_MSG_ERROR([atomictype $1 failed])
        fi
        )
    fi
  ]
)

AC_DEFUN([AC_CXXTOOLS_ATOMICTYPE],
[
  AC_MSG_CHECKING([atomic type])
  AC_ARG_WITH(
    [atomictype],
    AS_HELP_STRING([--with-atomictype],
                   [force atomic type. Accepted arguments:
                    sun, windows, att_x86, att_x86_64, att_arm, att_mips, att_ppc, att_sparc, pthread,
                    generic, probe]),
    [ ac_cxxtools_atomicity=$withval ],
    [ ac_cxxtools_atomicity=probe ])

  dnl check, if atomictype is valid

  dnl sun
  AC_CHECKATOMICTYPE([sun], [CXXTOOLS_ATOMICITY_SUN],
      [ #include <sys/atomic.h>
        int main () { volatile ulong_t* uvalue; atomic_inc_ulong_nv( uvalue ); } ])
  AC_CHECKATOMICTYPE([windows], [CXXTOOLS_ATOMICITY_WINDOWS],
      [ #define _WINSOCKAPI_
        #include <windows.h>
        int main() { LONG value; InterlockedIncrement(&value); } ])
  AC_CHECKATOMICTYPE([att_arm], [CXXTOOLS_ATOMICITY_GCC_ARM],
      [
        #include <csignal>
        typedef std::sig_atomic_t atomic_t;
        void atomicIncrement(volatile atomic_t& dest)
        {
               int a, b, c;

               asm volatile (  "0:\n\t"
                                       "ldr %0, [[%3]]\n\t"
                                       "add %1, %0, %4\n\t"
                                       "swp %2, %1, [[%3]]\n\t"
                                       "cmp %0, %2\n\t"
                                       "swpne %1, %2, [[%3]]\n\t"
                                       "bne 0b"
                                       : "=&r" (a), "=&r" (b), "=&r" (c)
                                       : "r" (&dest), "r" (1)
                                       : "cc", "memory");
        } ])
  AC_CHECKATOMICTYPE([att_mips], [CXXTOOLS_ATOMICITY_GCC_MIPS],
      [
        #include <csignal>
        typedef std::sig_atomic_t atomic_t;

        void atomicIncrement(volatile atomic_t& val)
        {
            atomic_t tmp, result = 0;

            asm volatile ("    .set    mips32\n"
                          "1:  ll      %0, %2\n"
                          "    addu    %1, %0, 1\n"
                          "    sc      %1, %2\n"
                          "    beqz    %1, 1b\n"
                          "    .set    mips0\n"
                          : "=&r" (result), "=&r" (tmp), "=m" (val)
                          : "m" (val));
        }
         ])
  AC_CHECKATOMICTYPE([att_ppc], [CXXTOOLS_ATOMICITY_GCC_PPC],
      [
        #include <csignal>
        typedef std::sig_atomic_t atomic_t;
        void atomicIncrement(volatile atomic_t& val)
        {
            atomic_t result = 0, tmp;
            asm volatile ("\n1:\n\t"
                          "lwarx %0, 0, %2\n\t"
                          "addi %1, %0, 1\n\t"
                          "stwcx. %1, 0, %2\n\t"
                          "bne- 1b\n"
                          "isync\n"
                          : "=&b" (result), "=&b" (tmp): "r" (&val): "cc", "memory");
        } ])
  AC_CHECKATOMICTYPE([att_sparc], [CXXTOOLS_ATOMICITY_GCC_SPARC],
      [
        #include <csignal>
        typedef std::sig_atomic_t atomic_t;
        void atomicIncrement(volatile atomic_t& val)
        {
            register volatile atomic_t* dest asm("g1") = &val;
            register atomic_t tmp asm("o4");
            register atomic_t ret asm("o5");

            asm volatile(
                    "1:     ld      [%%g1], %%o4\n\t"
                    "       add     %%o4, 1, %%o5\n\t"
                    /*      cas     [%%g1], %%o4, %%o5 */
                    "       .word   0xdbe0500c\n\t"
                    "       cmp     %%o4, %%o5\n\t"
                    "       bne     1b\n\t"
                    "        add    %%o5, 1, %%o5"
                    : "=&r" (tmp), "=&r" (ret)
                    : "r" (dest)
                    : "memory", "cc");
        } ])
  AC_CHECKATOMICTYPE([att_x86_64], [CXXTOOLS_ATOMICITY_GCC_X86_64],
      [ #include <unistd.h>
        typedef ssize_t atomic_t;
        void atomicIncrement(volatile atomic_t& val)
        {
                static const atomic_t d = 1;
                atomic_t tmp;

                asm volatile ("lock; xaddq %0, %1"
                              : "=r" (tmp), "=m" (val)
                              : "0" (d), "m" (val));
        } ])
  AC_CHECKATOMICTYPE([att_x86], [CXXTOOLS_ATOMICITY_GCC_X86],
      [ #include <csignal>
        typedef std::sig_atomic_t atomic_t;
        void atomicIncrement(volatile atomic_t& val)
        {
                atomic_t tmp;
                asm volatile ("lock; xaddl %0, %1"
                              : "=r" (tmp), "=m" (val)
                              : "0" (1), "m" (val));
        } ])
  AC_CHECKATOMICTYPE([att_avr32], [CXXTOOLS_ATOMICITY_GCC_AVR32],
      [ 
        typedef int atomic_t;
        void atomicIncrement(volatile atomic_t& val)
        {
                volatile uint8_t tmp;
                asm volatile(
                    "in %0, __SREG__"           "\n\t"
                    "cli"                       "\n\t"
                    "ld __tmp_reg__, %a1"       "\n\t"
                    "inc __tmp_reg__"           "\n\t"
                    "st %a1, __tmp_reg__"       "\n\t"
                    "out __SREG__, %0"          "\n\t"
                    : "=&r" (tmp)
                    : "e" (&val)
                    : "memory"
                );
        } ])
  if test "$ac_cxxtools_atomicity" = "pthread"
  then
    CXXTOOLS_ATOMICITY=CXXTOOLS_ATOMICITY_PTHREAD
  fi
  if test "$ac_cxxtools_atomicity" = "generic"
  then
    CXXTOOLS_ATOMICITY=CXXTOOLS_ATOMICITY_GENERIC
  fi
  if test "$CXXTOOLS_ATOMICITY" = ""
  then
    AC_MSG_ERROR([check for atomictype failed])
  else
    AC_MSG_RESULT($ac_cxxtools_atomicity)
  fi
])
