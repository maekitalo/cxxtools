Cxxtools
========

cxxtools is my toolbox with reusable c++-components

see INSTALL for installation-instructions


Installation from GIT
=====================

In the GIT-repository there is no configure-script, but configure.ac,
which is the source for configure. You need autoconf and automake to
create configure. 

Run `autoreconf -i` to generate a configure script.

Documentation
=============

Short howtos can be found here:

 * [parsing command line arguments](doc/argument-parsing-howto.markdown)
 * [using binary rpc](doc/binaryrpc-howto.markdown)
 * [details about handling connection in rpc clients](doc/connection.md)
 * [reading and writing CSV](doc/csv-howto.markdown)
 * [reading and writing JSON](doc/json-howto.markdown)
 * [using json rpc](doc/jsonrpc-howto.markdown)
 * [longging with cxxtools](doc/logging-howto.markdown)
 * [reading properties files](doc/propertiesquick-howto.markdown)
 * [rpc client and server](doc/rpc-howto.markdown)
 * [serialization framework](doc/serialization-howto.markdown)
 * [timezone](doc/timezone-howto.markdown)
 * [xmlrpc](doc/xmlrpc-howto.markdown)

There are also some useful demo applications, which shows examples how to
use various stuff from cxxtools. They can be found in the demo directory.

Tools
=====

Two command line tools are offered by cxxtools.

siconvert
---------

The tool `siconvert` allows converting between various formats.

A short introduction is printed when the tools is called without
arguments. Data can be read from standard in and output is written to
standard out. The tool can also be used to format files nicely.

The input format is typically passed as a small cap letter and the output
format with a capital letter. The flag -d enables nicely formatted output
(think of "display").

### Examples:

convert csv data to json:

    cat "foo.csv" | siconvert -cJd

convert (cxxtools-)binary data to xml

    cat "foo.bin" | siconvert -bXd

format a existing json file

    cat "foo.json" | siconvert -jJd

cxxtz
-----

The tool `cxxtz` is a command line tool to handle time zones. A short
introduction is printed when the tools is called without arguments.

### Examples:

List all known time zones of the system:

    cxxtz -a

Print the current time in the zime zone Pacific/Nauru

    cxxtz -c -z Pacific/Nauru
