Argument parsing with cxxtools
==============================

Frequenty when we write programs, we need to pass some command line arguments to
the process. It is very common to use command line switches, which start with a
minus sign followed by a character and optionally a parameter.

We all know, that applications in C++ start at the famous `main` function. As we
know, it takes 2 parameters (actually 3, but the 3rd is not that common): argc,
and argv.

There is a C function `getopt` in the standard library to extract the parameters
from those argc and argv but it is function quite C style and hence hard to use
and error prone.

Therefore _cxxtools_ has a alternative, which makes it very easy to extract
those parameters: the template class `cxxtools::Arg`. It not only reads the
parameter but also removes the consumed parameters from the argc/argv structure
and converts the value to a proper value.

It is defined in the header `<cxxtools/arg.h>`.

The class is typically instantiated on the stack just at the start of the `main`
function.

The constructor of the template class takes the type of the expected parameter
type as a type parameter.

The first parameter is a reference to the `argc` parameter of `main` and the
second a the `argv` parameter. Since `argv` is a pointer anyway it do not need
to be passed as a reference - it is already one.

The third parameter is either a character or a string. The character specifies
the option, which should be searched. Sometimes on character is not enough and
hence a string can be passed, which specifies the full option string.

The forth parameter is the default value, which is used, when the argument is
not found or could not be converted. The default value is optional. If no
default value is given, the value is default constructed when the argument was
not given.

`cxxtools::Arg` has a implicit conversion operator to the specified type to
access the value. Also a explicit method `getValue()` is available, which is
sometimes needed. Using the method `isSet()` returns true, when the option is
explicitly specified and the default value was not used.

Boolean switches
----------------

For boolean switches a template specialization is defined. The difference is,
that it do not take a default value, since the option is either given or not. It
just do not make sense to pass a default value.

Also no parameter is read but only checked whether the switch is there.

Multiple boolean switches can be grouped, so that they can be passed as a single
argument. E.g. passing _-xvc_ to the process and reading the switch 'v' leaves
_-xc_ on the command line.

Examples
--------

### short boolean switch

    int main(int argc, char* argv[])
    {
      cxxtools::Arg<bool> verbose(argc, argv, 'v');
      if (verbose)
        std::cout << "verbose option is set" << std::endl;
      for (int a = 1; a < argc; ++a)
        std::cout << "argv[" << a << "]=" << argv[a] << std::endl;
    }

When the program is called with the option _-v_, the condition is true and the
message is printed. The argument list is empty after that.

When the program is called with _foo -abv bar_, verbose is set again and the
output is:

    verbose option is set
    argv[1]=foo
    argv[2]=-ab
    argv[3]=bar

Note that the 'v' is removed from the group.

### long boolean switch

If you like longer arguments you may pass a string as noted alreads:

    int main(int argc, char* argv[])
    {
      cxxtools::Arg<bool> verbose(argc, argv, "--verbose");
      if (verbose)
        std::cout << "verbose option is set" << std::endl;
      for (int a = 1; a < argc; ++a)
        std::cout << "argv[" << a << "]=" << argv[a] << std::endl;
    }

Now you need to pass excactly this _--verbose_ as a separate parameter to enable
verbosity.

### other types

    int main(int argc, char* argv[])
    {
      cxxtools::Arg<std::string> host(argc, argv, 'h', "127.0.0.1");
      cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);
      std::cout << "host=" << host.getValue()
        << " port=" << port.getValue() << std::endl;
    }

This example extracts a host and a port parameter. Note that the explicit
`getValue()` method is needed since `cxxtools::Arg` has no output operator
defined and hence the compiler can't output it directly. But this is quite
seldom really needed since normally the value is just passed as a parameter. For
example to use the host and port to connect to a rpc server using a
`cxxtools::bin::RpcClient` you can use the implicit conversion of
`cxxtools::Arg` class:

    cxxtools::bin::RpcClient theClient(host, port);
