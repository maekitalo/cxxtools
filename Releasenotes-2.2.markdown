Releasenotes cxxtools 2.2
=========================

Logging is configured now with xml by default but may use other deserializable formats as well
----------------------------------------------------------------------------------------------
Previously logging was configured using a properties file format. Since the format is not really standard, a xml format is used now. The previous properties format is supported also to be compatible and it is also easier to edit that xml.

Move xml stuff into core library libcxxtools
--------------------------------------------
Since logging is now configured using xml, it was necessary to move the xml stuff into the core library. It makes also sense, since xml is a widely used format.

Improve iconv wrapper class
---------------------------
Iconv is a feature of the standard library for character conversion. Since the interface was quite difficult to use, cxxtools has had a simple wrapper for it for a long time. This wrapper is now improved to meake it even simpler for simple cases.

Almost rewrite unicode character class cxxtools::Char
-----------------------------------------------------
The class cxxtools::Char represents a unicode character. The class was rewritten in large parts to make it more robust and stringent when it comes to automatic conversion between Char and fundamental types.

Simplify use of codecs
----------------------
Static methods in codecs like the Utf8Codec makes using them simpler in simple cases.

New file device for asynchronous file I/O
-----------------------------------------
Cxxtools offers a asynchronous API which uses the signal slot feature. It was widely used in the network classes and rpc. Now also file I/O is supported.

Extend binary rpc protocol to support domains (similar namespace in C++)
------------------------------------------------------------------------
The binary rpc protocol now offers domains to group functions. This makes it more similar to xmlrpc or json over http, where functions can be grouped using different urls.

Better handling of SIGPIPE in network classes
---------------------------------------------
SIGPIPE is somewhat ugly in POSIX. By default a program stops using a signal when the peer closes the connection. This is unacceptable in larger applications. POSIX allows to ignore that signal. But this can be set only globally which is also bad for a library like cxxtools. But luckily there are extensions, which allow handling SIGPIPE per connections. These extensions are now used in cxxtools. No global handler for SIGPIPE need to be set any more.

Deseriailzer for property files
-------------------------------
A deserializer to read property files is implemented. This makes it easy to read complex structures from property files.

Implement simple API for parallel rpc calls
-------------------------------------------
Sometimes it is necessary to execute multiple rpc calls. And often it speeds up processing, when these requests are executed in parallel since the server normally run multithreaded or the request may even go to different servers, which run in parallel then. Previously this could be done using either threads or more lightweight using the asyncrounous rpc API. This API is quite difficult to handle since handlers for signals must be installed. Now a much simpler API is implemented. Multiple request can be initiated before waiting for results.
