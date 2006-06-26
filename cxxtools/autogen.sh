#! /bin/sh
# Regenerate the files autoconf / automake

libtoolize --force --automake --ltdl

rm -f config.cache config.log
aclocal
autoheader
autoconf
automake -a
