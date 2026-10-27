Argument parsing with cxxtools
==============================

When writing programs, we often need to pass command-line arguments to the
process. It is common to use command-line options that start with a minus sign,
followed by a character and, optionally, a value.

As we all know, C++ applications start at the `main` function. It usually takes
two parameters (`argc` and `argv`), although a third parameter is supported on
some systems.

There is a C function `getopt` for extracting options from `argc` and `argv`.
However, its interface is very C-like, making it somewhat cumbersome and
error-prone to use in C++.

cxxtools provides an alternative that makes extracting command-line arguments
easy: the template class `cxxtools::Arg`. It not only reads an argument, but
also removes the consumed arguments from `argc` and `argv` and converts the
value to the requested type.

It is defined in the header `<cxxtools/arg.h>`.

The class is typically instantiated on the stack at the beginning of the
`main` function.

The template parameter specifies the expected type of the option value.

The first constructor parameter is a reference to the `argc` parameter of
`main`, and the second is the `argv` parameter. Since `argv` is already a
pointer, it does not need to be passed by reference.

The third parameter is either a character or a string. A character specifies
a short option to search for. If a single-character option is not sufficient,
a string can be passed to specify the complete option name.

The fourth parameter is the default value, which is used when the argument is
not found or cannot be converted. The default value is optional. If no default
value is provided, the value is default-constructed when the argument is not
specified.

`cxxtools::Arg` provides an implicit conversion operator to the specified type,
which can be used to access the value. An explicit `getValue()` method is also
available and is useful in some situations.

The `isSet()` method returns `true` if the option was explicitly specified and
the default value was not used.

Boolean switches
----------------

A template specialization is provided for boolean switches. Unlike the general
case, it does not take a default value because the option is either present or
absent.

No option value is read. Instead, `cxxtools::Arg<bool>` only checks whether the
switch is present.

Multiple boolean switches can be grouped and passed as a single argument. For
example, if `-xvc` is passed to the process and the `v` switch is read, `-xc`
remains on the command line.

Examples
--------

### Short boolean switch

```cpp
int main(int argc, char* argv[])
{
    cxxtools::Arg<bool> verbose(argc, argv, 'v');

    if (verbose)
        std::cout << "verbose option is set" << std::endl;

    for (int a = 1; a < argc; ++a)
        std::cout << "argv[" << a << "]=" << argv[a] << std::endl;
}
```

When the program is called with the option `-v`, the condition is true and the
message is printed. The option is removed from the argument list.

When the program is called with `foo -abv bar`, `verbose` is set again and the
output is:

```text
verbose option is set
argv[1]=foo
argv[2]=-ab
argv[3]=bar
```

Note that the `v` option is removed from the group.

### Long boolean switch

If you prefer longer option names, you can pass a string instead:

```cpp
int main(int argc, char* argv[])
{
    cxxtools::Arg<bool> verbose(argc, argv, "--verbose");

    if (verbose)
        std::cout << "verbose option is set" << std::cout;
```

Standard input and standard output
---------------------------------

Many command-line tools read data from an input file and write results to an
output file. It is common practice to make both optional. When no input file
is specified, the tool reads from standard input. Likewise, when no output
file is specified, the result is written to standard output.

cxxtools provides two helper classes for this purpose: `cxxtools::ArgIn` and
`cxxtools::ArgOut`.

`cxxtools::ArgIn` is derived from `std::istream`. It looks for an input file
name in the command-line arguments. If a file name is found, the file is
opened and the corresponding argument is removed from `argc` and `argv`.
Otherwise, the stream is connected to `std::cin`.

This allows applications to handle file input and standard input in the same
way:

```cpp
int main(int argc, const char* argv[])
{
    cxxtools::ArgIn in(argc, argv);

    std::string line;
    while (std::getline(in, line))
        std::cout << line << std::endl;
}
```

If the input file should be provided through a specific option, a short or
long option can be specified:

```cpp
cxxtools::ArgIn in(argc, argv, 'i');
```

or

```cpp
cxxtools::ArgIn in(argc, argv, "--input");
```

`cxxtools::ArgOut` works in the same way. It is derived from `std::ostream`.
When an output file is specified, it opens that file. Otherwise, it writes to
`std::cout`.

```cpp
int main(int argc, const char* argv[])
{
    cxxtools::ArgOut out(argc, argv);

    out << "Hello World" << std::endl;
}
```

Again, the output file can optionally be specified through a short or long
option:

```cpp
cxxtools::ArgOut out(argc, argv, 'o');
```

or

```cpp
cxxtools::ArgOut out(argc, argv, "--output");
```

A common pattern for command-line tools is therefore:

```cpp
int main(int argc, const char* argv[])
{
    cxxtools::ArgIn in(argc, argv);
    cxxtools::ArgOut out(argc, argv);

    // process data
}
```

The application automatically reads from files when file names are provided
and otherwise falls back to standard input and standard output.
