Logging with cxxtools
=====================

Frequently C++ applications need to log some output to inform about special
situations, warning or error conditions. Or just for debugging purpose it would
be nice, if a application can be put into some mode, where it writes additional
informations to somewhere.

_cxxtools_ has such a feature, where logging can be built easily into own
applications and also configured easily. _cxxtools_ itself has many debug
output, which may be enabled if something weird happens, which need to be
analyzed.

The logging facility brings some nice features, which makes it really flexible.
The features are:

 * disabled logging is very cheap, so that it is no problem to spread log
   statements in the code and disable them for production
 * logging can be configured using a configuration file or using cxxtools
   serialization
 * log output can be printed to the standard output, a file or sent via udp to a
   remote host
 * when logging to a file, a maximum size of the log file can be configured
   after which the file is renamed and a new one started
 * each log statement has a level and a category
 * for each category it is possible to configure the log level, which specifies
   up to which log level the logging is printed
 * categories are organized hierarchically
 * formatting log messages is easy, since a std::ostream is used automatically

Category
--------

As said each log output belongs to a category. Typically each source file has
its own category. The category is specified with a string. Subcategories can be
defined. A subcategory is separated by a dot from the upper level category.
Typically a category is defined per namespace.

For example lets say we have a project, where the classes are defined in the
namespace `myapp`. The top level category should be this `myapp`. The second
level category is typically the class or file name, which is often the same. When
we have a class `TheCar` in the namespace `myapp`, the category should be
_myapp.TheCar_. Of course you are free to define a different naming scheme.

To define a category include the file `<cxxtools/log.h>` in your cpp file and
put a line `log_define(myapp.TheCar)` at the top of the file.  This defines the
category.

Logging
-------

Each log statement has a level. The levels are in descending severity _fatal_,
_error_, _warn_, _info_ and _debug_. Additionally there is a special level
_trace_, which is even below _debug_ and works a little different. But lets
look first, how we can produce log output.

Lets say we want to output the number of wheels of our car as a
debug information. And the number of wheels are held in a variable
`numberOfWheels`. We produce a debug output by putting this into our source:

    log_debug("the number of wheels is " << numberOfWheels);

Now when the debug or trace level is defined for our category, the text is
output into our log target.

Note that you can output anything, which has a proper output operator for
std::ostream defined.

The other levels are produced similarly using the macros `log_fatal`,
`log_error`, `log_warn` or `log_info`.

Tracing is a little different. The macro `log_trace` instantiates a class on the
stack, which logs the output prefixed with the text _ENTER_ and when destroyed
at the end of the scope with the text _EXIT_. When enabled it can easily be
seen whether the code is inside a scope or not.

`cxxtools` itself uses always the top level category _cxxtools_ for logging and
makes extensive use of subcategories. So if you want to watch, what cxxtools
does, you can enable debug log for category _cxxtools_.

Configuration
-------------

Configuration, which categories are logged with which level and where to log is
normally done using a configuration file. The easiest way to initialize logging
in a application is to call the macro `log_init()`. Typically this is done as
the first statement in the `main` function of a application.

This macro looks for different variants of configuration files. The
configuration file format may be xml, properties or in the future (cxxtools
version > 2.2) json. First it looks for a file named _log.xml_. If found,
configuration is done by reading that xml file. If not, it looks for a file
_log.properties_. This was the only variant in cxxtools version below 2.2. In
cxxtools version > 2.2 it will look for a file _log.json_ in json format. If
nothing is found, the logging is not configured and no output is produced.

There are 2 additional variants. `log_init()` takes optionally a parameter. It
is either a file name or a `cxxtools::SerializationInfo`. If the file extension
is `.properties`, properties format is again used. If the file extension is
`.json` (for cxxtools version > 2.2), json format is used. Otherwise xml is
assumed.

The variant with `cxxtools::SerializationInfo` can be used to either build your
own configuration without a file or using one of the deserializers in _cxxtools_
and extract only a part of a configuration file.

### Format: xml

So lets look at a typical xml configuration file:

    <?xml version="1.0" encoding="UTF-8"?>
    <logging>
      <rootlogger>INFO</rootlogger>
      <loggers>
        <logger>
          <category>cxxtools.csv</category>
          <level>DEBUG</level>
        </logger>
        <logger>
          <category>myapp.TheCar</category>
          <level>DEBUG</level>
        </logger>
      </loggers>
      <file>debug.log</file>
      <maxfilesize>1MB</maxfilesize>
      <maxbackupindex>2</maxbackupindex>
    </logging>

The rootlogger node defined the log level for all categories, which are not
specified explicitly. In this case log output for levels _fatal_, _error_,
_warn_ and _info_ is enabled.

Then there is a list of loggers entries. Each entry has a category and a level.
They define the levels for each category and all its subcategories. So in this
case you will get all levels up to debug for the specified categories. You will
see debug output also for categories _cxxtools.csv.parser_ and
_cxxtools.csv.formatter_.

The level may be _FATAL_, _ERROR_, _WARN_, _INFO_, _DEBUG_ or _TRACE_. Actually
only the first character is significant and even that is not case sensitive, so
you can write as well `<level>d</level>` to define _debug_ level.

The _file_ entry enabled logging to a file. I guess you guessed it already.

When a file is defined and both _maxfilesize_ and _maxbackupindex_ the file is
automatically rotated.

When the specified maximum file size is exceeded, the current file gets the
suffix _.0_ and a new file is created. If there was a previous file with the
suffix _.0_ it is renamed to _.1_ and so on until the specified maximum backup
index is reached. The last file is just deleted.

The _maxfilesize_ may be just a number, then it is a number of bytes. Optionally
it can be followed by a suffix 'k', 'm' or 'g' (case does not matter), in which
case the number is interpreted as kilobytes, megabytes or gigabytes. The rest of
the entry is just ignored. So you can write _1M_ or _1MB_ or _1megabyte_, which
all means the same.

Note that the file will get always a little larger since the log entries are not
cut hard but rotation takes place, when the maximum size was exceeded.

The entry _maxbackupindex_ specifies the maximum index when files are rotated.

In this case we need up to slightly more than 4MB file space is consumed for all
log files. The current file and the backup files _*.0_, _*.1_ and _*.2_.

Ordering of the nodes is not significant and also nodes, which are unknown are
just ignored.

If you remove the `<file>`, `<maxfilesize>` and `<maxbackupindex>` nodes, the
debug output is printed to standard error. If you add node `<stdout>1</stdout>`
or `<stdout>true</stdout>` standard output is used.

The node `<host>somehost:1234</host>` sends log output via udp to the specified
udp port.

### Format: properties

The properties file format was the only format supported by cxxtools prior 2.2.
It is easier to write and read but less standard than xml and also more
difficult to process automatically.

Here is the same configuration as above in properties format:

    rootlogger = INFO
    logger.cxxtools.csv = DEBUG
    logger.myapp.TheCar = DEBUG
    file = debug.log
    maxfilesize = 1MB
    maxbackupindex = 2

This is straight forward. It is more or less a assignment of values to
variables. The meaning is exactly the same as in xml format. Also logging via
udp can be configured by specifying a variable `host` with a host and port
number separated by ':' as in xml.

### Format: json

The json format is supported for cxxtools version later than 2.2. Since it is so
similar to xml, just the same configuration is shown in json here without
explanation:

    {
      rootlogger: "INFO",
      loggers: [
        {
          category: "cxxtools.csv",
          level: "DEBUG"
        },
        {
          category: "myapp.TheCar",
          level: "DEBUG"
        }
      ],
      file: "debug.log",
      maxfilesize: "1MB",
      maxbackupindex: 2
    }

Note that json is slightly extended since the keys of a structure need not be
enclosed in quotes.
