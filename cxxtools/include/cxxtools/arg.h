/* cxxtools/arg.h
   Copyright (C) 2003 Tommi MÃ¤kitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_ARG_H
#define CXXTOOLS_ARG_H

#include <sstream>

namespace cxxtools
{

////////////////////////////////////////////////////////////////////////
/**
 Read and extract commandline parameters from argc/argv.

 Programs usually need some parameters. Usually they start with a '-'
 followed by a single character and optionally a value.
 arg<T> extracts these and other parametes.

 This default class processes paramters with a value, which defines
 a input-extractor-operator operator>> (istream&, T&).

 Options are removed from the option-list, so programs can easily check
 after all options are extracted, if there are parameters left.

 example:
 \code
   int main(int argc, char* argv[])
   {
     cxxtools::arg<int> option_n(argc, argv, 'n', 0);
     std::cout << "value for -n: " << option_n << endl;
   }
 \endcode

 */
template <class T>
class arg
{
  public:
    /**
     extract parameter.

     \param argc      1. parameter of main
     \param argv      2. of main
     \param ch        optioncharacter
     \param def       default-value

     example:
     \code
      arg<unsigned> offset(argc, argv, 'o', 0);
      unsigned value = offset.getValue();
     \endcode
     */
    arg(int& argc, char* argv[], char ch, const T& def = T())
    {
      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0'
          && !( std::istringstream(argv[i + 1]) >> m_value).fail()
          )
        {
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }
      
      m_isset = false;
      m_value = def;
    }

    /**
     GNU defines long options starting with "--". This (and more) is
     supported here. Instead of giving a single option-character, you
     specify a string.

     example:
     \code
       arg<int> option_number(argc, argv, "--number", 0);
       std::cout << "number =" << option_nummer.getValue() << std::endl;
     \endcode
     */
    arg(int& argc, char* argv[], const char* str, const T& def = T())
    {
      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0
          && !( std::istringstream(argv[i + 1]) >> m_value).fail()
          )
        {
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }

      m_isset = false;
      m_value = def;
    }

    /**
     Reads next parameter and removes it.
     */
    arg(int& argc, char* argv[])
    {
      if (argc > 1
          && !( std::istringstream(argv[1]) >> m_value).fail()
          )
      {
        m_isset = true;
        for (int i = 1; i < argc - 1; ++i)
          argv[i] = argv[i + 1];
        argc -= 1;
        argv[argc] = 0;
      }
      else
        m_isset = false;
    }

    /**
     returns the value.
     */
    const T& getValue() const   { return m_value; }

    /**
     returns the value.

     Instead of calling getValue() the argument can be converted
     implicitely.

     example:

     \code
     void print(int i)
     { std::cout << i << std::endl; }

     int main(int argc, char* argv[])
     {
       cxxtools::arg<int> value(argc, argv, 'v', 0);
       print(value);   // pass argument as a int to the function
     }
     \endcode
     */
    operator T() const   { return m_value; }

    /**
     * returns true, if the option is set and the default-value was not
     * used
     */
    bool isSet() const   { return m_isset; }

  private:
    T    m_value;
    bool m_isset;
};

////////////////////////////////////////////////////////////////////////
/**
 specialization for bool.

 Often programs need some switches, which are switched on or off.
 Users just enter a option without parameter.

 example:
 \code
   arg<bool> debug(argc, argv, 'd');
   if (debug)
     std::cout << "debug-mode is set" << std::endl;
 \endcode
 */
template <>
class arg<bool>
{
  public:
    /**
     Use this constructor to extract a bool-parameter.

     As a special case options can be grouped. The parameter is
     recognized also in a argument, which starts with a '-' and contains
     somewhere the given character.

     example:
     \code
      arg<bool> debug(argc, argv, 'd');
      arg<bool> ignore(argc, argv, 'i');
     \endcode

     Arguments debug and ignore are both set when the program is called
     with:
     \code
      prog -id

      prog -i -d
     \endcode

     Options can also switched off with a following '-' like this:
     \code
      prog -d-
     \endcode

     In the program use:
     \code
      arg<bool> debug(argc, argv, 'd');
      if (debug.isSet())
      {
        if (debug)
          std::cout << "you entered -d" << std::endl;
        else
          std::cout << "you entered -d-" << std::endl;
      }
      else
        std::cout << "no -d option given" << std::endl;
     \endcode

     This is useful, if programs defaults to some enabled feature,
     which can be disabled.
     */
    arg(int& argc, char* argv[], char ch)
    {
      for (int i = 1; i < argc; ++i)
        if (argv[i][0] == '-' && argv[i][1] != '-')
        {
          // fängt mit einem '-' an, aber nicht mit '--'
          if (argv[i][1] == ch && argv[i][2] == '\0')
          {
            // das ist eine allein stehende Option
            m_value = true;
            m_isset = true;
            for ( ; i < argc - 1; ++i)
              argv[i] = argv[i + 1];
            argc -= 1;
            argv[argc] = 0;
            return;
          }
          else if (argv[i][1] == ch && argv[i][2] == '-' && argv[i][3] == '\0')
          {
            // Option wurde explizit mit -x- ausgeschaltet
            m_value = false;
            m_isset = true;
            for ( ; i < argc - 1; ++i)
              argv[i] = argv[i + 1];
            argc -= 1;
            argv[argc] = 0;
            return;
          }
          else
          {
            // allein steht das nicht, aber mal sehen, ob der Optionsbuchstabe
            // in der Gruppe vorkommt
            for (char* p = argv[i] + 1; *p != '\0'; ++p)
              if (*p == ch)
              {
                // na da haben wir ihn; weg damit und fertig
                m_value = true;
                m_isset = true;
                do
                {
                  *p = *(p + 1);
                } while (*p++ != '\0');

                return;
              }
          }
        }
      m_value = false;
      m_isset = false;
    }

    /**
     Constructor for long-options.

     The option-parameter is defined with a string. This can extract
     long-options like:
     \code
       prog --debug
     \endcode

     with
     \code
       arg<bool> debug(argc, argv, "--debug");
     \endcode

     */
    arg(int& argc, char* argv[], const char* str)
    {
      for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = true;
          m_isset = true;
          for ( ; i < argc - 1; ++i)
            argv[i] = argv[i + 1];
          argc -= 1;
          argv[argc] = 0;
          return;
        }
      m_value = false;
      m_isset = false;
    }

    /**
     returns true, if options is set.
     */
    bool isTrue() const   { return m_value; }

    /**
     returns true, if options is not set.
     */
    bool isFalse() const  { return !m_value; }

    /**
     convertable to bool.
     */
    operator bool() const  { return m_value; }

    /**
     returns true, if option is explicitly set
     */
    bool isSet() const             { return m_isset; }

  private:
    bool m_value;
    bool m_isset;
};

////////////////////////////////////////////////////////////////////////
/**
 Special handling for "const char*".

 "const char*" is not extracted with a stream. This is more flexible
 and easier to process. Also parameters can contain spaces.
 */
template <>
class arg<const char*>
{
  public:
    /**
     Constructor for the short form.
     */
    arg(int& argc, char* argv[], char ch, const char* def = 0)
    {
      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0')
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }

      m_isset = false;
      m_value = def;
    }

    /**
     Constructor for the long form.
     */
    arg(int& argc, char* argv[], const char* str, const char* def = 0)
    {
      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }

      m_isset = false;
      m_value = def;
    }

    /**
     Extracts the next parameter.
     */
    arg(int& argc, char* argv[])
    {
      if (argc > 1)
      {
        m_value = argv[1];
        m_isset = true;
        for (int i = 1; i < argc - 1; ++i)
          argv[i] = argv[i + 1];
        argc -= 1;
        argv[argc] = 0;
      }
      else
      {
        m_value = 0;
        m_isset = false;
      }
    }

    /**
     returns the extracted value.
     */
    const char* getValue() const   { return m_value; }

    /**
     argument is convertible to "const char*"
     */
    operator const char*() const   { return m_value; }

    /**
     returns true, when the option is not set and the default is used.
     */
    bool isSet() const             { return m_isset; }

  private:
    const char* m_value;
    bool        m_isset;
};

}

#endif // CXXTOOLS_ARG_H
