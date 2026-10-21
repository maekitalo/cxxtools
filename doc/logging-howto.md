Logging with cxxtools
=====================

C++ applications often need to log messages about special situations, warnings,
or errors. Logging is also useful for debugging, where an application can be
configured to produce additional diagnostic information.

cxxtools provides a logging facility that can easily be integrated into an
application and configured as needed. cxxtools itself also contains extensive
debug logging that can be enabled when unexpected behavior needs to be
analyzed.

The logging facility provides several useful features:

- disabled logging has very little overhead, making it practical to add log
  statements throughout the code and disable them in production
- logging can be configured using a configuration file or cxxtools
  serialization
- log output can be written to standard output, a file, or sent via UDP to a
  remote host
- when logging to a file, a maximum file size can be configured; when this size
  is exceeded, the file is rotated and a new file is created
- each log statement has a level and a category
- for each category, a log level can be configured to specify which messages
  are written
- categories are organized hierarchically
- formatting log messages is easy because `std::ostream` is used automatically

Category
--------

Each log message belongs to a category. Typically, each source file has its own
category. A category is specified as a string. Categories can have
subcategories, separated from their parent category by a dot. Typically, the
top-level category corresponds to a namespace.

For example, suppose we have a project whose classes are defined in the
namespace `myapp`. The top-level category could be `myapp`. The second-level
category is typically the class or file name, which is often the same.

For a class `TheCar` in the namespace `myapp`, the category could therefore be
`myapp.TheCar`. Of course, you are free to use a different naming scheme.

To define a category, include `<cxxtools/log.h>` in your `.cpp` file and add the
following line near the top of the file:

```cpp
log_define(myapp.TheCar)
```

This defines the category for that file.

Logging
-------

Each log statement has a level. The levels, in descending order of severity,
are `fatal`, `error`, `warn`, `info`, and `debug`. There is also a special
`trace` level, which is below `debug` and behaves somewhat differently.

The additional levels `fine`, `finer`, and `finest` allow debug output to be
controlled more granularly. `fine` is equivalent to `debug`.

Let's first look at how to produce log output.

Suppose we want to log the number of wheels of our car as debugging
information, and the number of wheels is stored in a variable named
`numberOfWheels`. We can produce debug output using the corresponding logging
macro.

```cpp
    log_debug("number of wheels: " << numberOfWheels);
```

When the debug or trace level is enabled for our category, the message is
written to the configured log target.

You can log anything for which a suitable output operator for `std::ostream`
is defined.

The other levels are used in the same way through the macros `log_fatal`,
`log_error`, `log_warn`, and `log_info`.

Tracing works somewhat differently. The `log_trace` macro instantiates an
object on the stack. It logs a message prefixed with `ENTER` and, when the
object is destroyed at the end of the scope, logs another message prefixed
with `EXIT`. When tracing is enabled, this makes it easy to see whether
execution is currently inside a particular scope.

cxxtools itself always uses `cxxtools` as its top-level logging category and
makes extensive use of subcategories. To see what cxxtools is doing, you can
enable debug logging for the `cxxtools` category.

Configuration
-------------

The configuration determines which categories are logged at which levels and
where the log output is written. This is normally specified in a configuration
file.

The easiest way to initialize logging in an application is to call the
`log_init()` macro. Typically, this is done near the beginning of the `main`
function.

The macro looks for several variants of configuration files. The configuration
file can use the properties, JSON, or XML format.

First, `log_init()` looks for a file named `log.properties`. If it is found,
the logging configuration is read from that file. Otherwise, it looks for
`log.json` or `log.xml`.

There are four additional variants. `log_init()` optionally takes one or two
parameters. A single parameter can be either a file name or an object of type
`cxxtools::LogConfiguration`.

With two parameters, `log_init()` expects the `argc` and `argv` parameters from
`main` and uses them to find the configuration file. It looks for a file named
after the process with the extension `.properties`, `.json`, or `.xml`,
whichever is found first. If no such file is found, `log_init()` without
parameters is used.

If the file extension is `.properties`, the properties format is used. If the
extension is `.json`, the JSON format is used. If the extension is `.xml`, the
XML format is used. Otherwise, an error message is written to standard error.

An object of type `cxxtools::LogConfiguration` can be used to read the logging
configuration from your own file using a suitable deserializer or to construct
a configuration programmatically.

The currently active configuration can be retrieved using:

```cpp
cxxtools::LogManager::getInstance().getLogConfiguration()
```

A copy of the configuration is returned. `log_init()` can be called multiple
times to change the active logging configuration at runtime.

Format: XML
-----------

Let's look at a typical XML configuration file:

```xml
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
```

The `rootlogger` element defines the log level for all categories that are not
configured explicitly. In this example, messages with the levels `fatal`,
`error`, `warn`, and `info` are enabled.

The `loggers` element contains a list of logger entries. Each entry has a
category and a level. The configured level applies to the specified category
and all of its subcategories.

In this example, all levels up to `debug` are enabled for the specified
categories. This also enables debug output for subcategories such as
`cxxtools.csv.parser` and `cxxtools.csv.formatter`.

The level may be `FATAL`, `ERROR`, `WARN`, `INFO`, `DEBUG`, or `TRACE`. Only
the first character is significant, and it is not case-sensitive. Therefore,
for example,

```xml
<level>d</level>
```

can also be used to select the `debug` level.

There are also the levels `FINE`, `FINER`, and `FINEST`, which are not enabled
by `TRACE`. Individual levels can also be enabled separately. There are four
different ways to specify the desired logging levels.

The simplest form is to specify a log level. As a special case, `TRACE` does
not enable `FINER` and `FINEST`.

A log level can be prefixed with `T` to add trace output to the selected log
level. For example, `TINFO` logs all output enabled by `INFO` and additionally
enables the `TRACE` level.

Individual log levels can be specified by separating them with `|`. For
example, a configuration selecting `FATAL`, `INFO`, and `TRACE` enables exactly
those levels, but not `WARN`, `ERROR`, `DEBUG`, `FINE`, `FINER`, or `FINEST`.

The logging configuration also supports selecting a single individual level
by prefixing it with `|`.

The `file` entry enables logging to a file. The file name may contain
environment variables, for example:

```text
/home/$USER/log/foo.log
```

See the class `cxxtools::EnvSubst` for details.

When a file is configured and `maxfilesize` is specified, the log file is
automatically rotated.

When the configured maximum file size is exceeded, the current file is renamed
with the suffix `.0`, and a new file is created. If a previous file with the
suffix `.0` exists, it is renamed to `.1`, and so on until the configured
maximum backup index is reached. The oldest file is then deleted.

`maxfilesize` may be specified as a number, in which case it represents a
number of bytes. It can optionally be followed by the suffix `k`, `m`, or `g`
(case-insensitive), in which case the number is interpreted as kilobytes,
megabytes, or gigabytes. Any remaining characters in the value are ignored.
Therefore, `1M`, `1MB`, and `1megabyte` all have the same meaning.

Note that a log file may become slightly larger than the configured maximum
size. Log entries are not truncated; rotation takes place after the maximum
size has been exceeded.

The `maxbackupindex` entry specifies the maximum backup index used when log
files are rotated and defaults to 0.

In this example, slightly more than 4 MB of disk space may be used by all log
files together: the current file and the three backup files `.0`, `.1`, and
`.2`.

The order of the elements is not significant, and unknown elements are
ignored.

If the `file`, `maxfilesize`, and `maxbackupindex` elements are omitted, the
log output is written to standard error.

If either of the following elements is added,

```xml
<stdout>1</stdout>
```

or

```xml
<stdout>true</stdout>
```

standard output is used instead.

The element

```xml
<host>somehost:1234</host>
```

sends log output via UDP to the specified host and UDP port.

Format: properties
------------------

The properties file format is the default format. It is easier to write and
read than XML, although it is less standardized and more difficult to process
automatically.

Here is the same configuration in properties format:

```properties
rootlogger = INFO
logger.cxxtools.csv = DEBUG
logger.myapp.TheCar = DEBUG
file = debug.log
maxfilesize = 1MB
maxbackupindex = 2
```

The format is straightforward. It essentially consists of assignments of
values to configuration variables. Their meaning is the same as in the XML
format.

Logging via UDP can also be configured by specifying the `host` property with
a host name and port separated by `:`, just as in the XML configuration.

Format: JSON
------------

The JSON configuration format is similar to the XML format but is more concise.

Here is the same configuration again:

```json
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
```

Note that cxxtools uses a slightly extended JSON syntax here: keys in an object
do not have to be enclosed in quotation marks.
