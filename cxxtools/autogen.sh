#! /bin/sh
# Regenerate the files autoconf / automake

libtoolize --force --automake --ltdl

rm -f config.cache config.log
aclocal -I m4
autoheader
autoconf
automake -a
