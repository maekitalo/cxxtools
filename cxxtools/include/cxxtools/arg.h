/* cxxtools/arg.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003,2004 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CXXTOOLS_ARG_H
#define CXXTOOLS_ARG_H

#include <sstream>
#include <string.h>

namespace cxxtools
{

////////////////////////////////////////////////////////////////////////
/**
 Read and extract commandline parameters from argc/argv.

 Programs usually need some parameters. Usually they start with a '-'
 followed by a single character and optionally a value.
 Arg<T> extracts these and other parametes.

 This default class processes paramters with a value, which defines
 a input-extractor-operator operator>> (istream&, T&).

 Options are removed from the option-list, so programs can easily check
 after all options are extracted, if there are parameters left.

 example:
 \code
   int main(int argc, char* argv[])
   {
     cxxtools::Arg<int> option_n(argc, argv, 'n', 0);
     std::cout << "value for -n: " << option_n << endl;
   }
 \endcode

 */
template <typename T>
class Arg
{
  public:
    /**
     default constructor. Initializes value.

     \param def    initial value
     */
    Arg(const T& def = T())
      : m_value(def),
        m_isset(false)
      { }

    /**
     extract parameter.

     \param argc      1. parameter of main
     \param argv      2. of main
     \param ch        optioncharacter
     \param def       default-value

     example:
     \code
      Arg<unsigned> offset(argc, argv, 'o', 0);
      unsigned value = offset.getValue();
     \endcode
     */
    Arg(int& argc, char* argv[], char ch, const T& def = T())
      : m_value(def),
        m_isset(false)
    {
      set(argc, argv, ch);
    }

    /**
     GNU defines long options starting with "--". This (and more) is
     supported here. Instead of giving a single option-character, you
     specify a string.

     example:
     \code
       Arg<int> option_number(argc, argv, "--number", 0);
       std::cout << "number =" << option_number.getValue() << std::endl;
     \endcode
     */
    Arg(int& argc, char* argv[], const char* str, const T& def = T())
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, str);
    }

    Arg(int& argc, char* argv[])
      : m_isset(false)
    {
      m_isset = set(argc, argv);
    }

    /**
     extract parameter.

     \param argc      1. parameter of main
     \param argv      2. of main
     \param ch        optioncharacter

     example:
     \code
      Arg<unsigned> offset;
      offset.set(argc, argv, 'o');
      unsigned value = offset.getValue();
     \endcode
     */
    bool set(int& argc, char* argv[], char ch)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0')
        {
          std::istringstream s(argv[i + 1]);
          s >> m_value;
          if (!s.fail())
          {
            m_isset = true;
            for ( ; i < argc - 2; ++i)
              argv[i] = argv[i + 2];
            argc -= 2;
            argv[argc] = 0;
            return true;
          }
        }
      
      return false;
    }

    /**
     GNU defines long options starting with "--". This (and more) is
     supported here. Instead of giving a single option-character, you
     specify a string.

     example:
     \code
       Arg<int> option_number;
       number.set(argc, argv, "--number");
       std::cout << "number =" << option_number.getValue() << std::endl;
     \endcode
     */
    bool set(int& argc, char* argv[], const char* str)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          std::istringstream s(argv[i + 1]);
          s >> m_value;
          if (!s.fail())
          {
            m_isset = true;
            for ( ; i < argc - 2; ++i)
              argv[i] = argv[i + 2];
            argc -= 2;
            argv[argc] = 0;
            return true;
          }
        }

      return false;
    }

    /**
     Reads next parameter and removes it.
     */
    bool set(int& argc, char* argv[])
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      if (argc > 1)
      {
        std::istringstream s(argv[1]);
        s >> m_value;
        if (!s.fail())
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
      else
        m_isset = false;

      return m_isset;
    }

    /**
     returns the value.
     */
    const T& getValue() const   { return m_value; }

    /**
     returns the value.

     Instead of calling getValue() the argument can be converted
     implicitly.

     example:

     \code
     void print(int i)
     { std::cout << i << std::endl; }

     int main(int argc, char* argv[])
     {
       cxxtools::Arg<int> value(argc, argv, 'v', 0);
       print(value);   // pass argument as a int to the function
     }
     \endcode
     */
    operator T() const   { return m_value; }

    /**
     * Liefert true zurück, wenn die Option gefunden wurde, also nicht
     * der Default-Wert zum Einsatz kam.
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
   Arg<bool> debug(argc, argv, 'd');
   if (debug)
     std::cout << "debug-mode is set" << std::endl;
 \endcode
 */
template <>
class Arg<bool>
{
  public:
    /**
     default constructor. Initializes value.

     \param def    initial value
     */
    Arg(bool def = false)
      : m_value(def),
        m_isset(false)
        { }

    /**
     extract parameter.

     \param argc      1. parameter of main
     \param argv      2. of main
     \param ch        optioncharacter
     \param def       default-value

     example:
     \code
      Arg<unsigned> offset(argc, argv, 'o', 0);
      unsigned value = offset.getValue();
     \endcode
     */
    Arg(int& argc, char* argv[], char ch, bool def = false)
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, ch);
    }

    Arg(int& argc, char* argv[], const char* str, bool def = false)
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, str);
    }

    /**
     Use this constructor to extract a bool-parameter.

     As a special case options can be grouped. The parameter is
     recognized also in a argument, which starts with a '-' and contains
     somewhere the given character.

     example:
     \code
      Arg<bool> debug(argc, argv, 'd');
      Arg<bool> ignore(argc, argv, 'i');
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
      Arg<bool> debug(argc, argv, 'd');
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

     This is useful, if a program defaults to some enabled feature,
     which can be disabled.
     */
    bool set(int& argc, char* argv[], char ch)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc; ++i)
        if (argv[i][0] == '-' && argv[i][1] != '-')
        {
          // starts with a '-', but not with "--"
          if (argv[i][1] == ch && argv[i][2] == '\0')
          {
            // single option found
            m_value = true;
            m_isset = true;
            for ( ; i < argc - 1; ++i)
              argv[i] = argv[i + 1];
            argc -= 1;
            argv[argc] = 0;
            return true;
          }
          else if (argv[i][1] == ch && argv[i][2] == '-' && argv[i][3] == '\0')
          {
            // Option was explicitly disabled with -x-
            m_value = false;
            m_isset = true;
            for ( ; i < argc - 1; ++i)
              argv[i] = argv[i + 1];
            argc -= 1;
            argv[argc] = 0;
            return true;
          }
          else
          {
            // look, if we find the option in a optiongroup
            for (char* p = argv[i] + 1; *p != '\0'; ++p)
              if (*p == ch)
              {
                // her it is - extract it
                m_value = true;
                m_isset = true;
                do
                {
                  *p = *(p + 1);
                } while (*p++ != '\0');

                return true;
              }
          }
        }

      return false;
    }

    /**
     Setter for long-options.

     The option-parameter is defined with a string. This can extract
     long-options like:
     \code
       prog --debug
     \endcode

     with
     \code
       Arg<bool> debug(argc, argv, "--debug");
     \endcode

     */
    bool set(int& argc, char* argv[], const char* str)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = true;
          m_isset = true;
          for ( ; i < argc - 1; ++i)
            argv[i] = argv[i + 1];
          argc -= 1;
          argv[argc] = 0;
          return true;
        }

      return false;
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
class Arg<const char*>
{
  public:
    Arg(const char* def = 0)
      : m_value(def),
        m_isset(false)
    { }

    Arg(int& argc, char* argv[], char ch, const char* def = 0)
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, ch);
    }

    Arg(int& argc, char* argv[], const char* str, const char* def = 0)
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, str);
    }

    Arg(int& argc, char* argv[])
      : m_value(0),
        m_isset(false)
    { 
      m_isset = set(argc, argv);
    }

    /**
     Constructor for the short form.
     */
    bool set(int& argc, char* argv[], char ch)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0')
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return true;
        }

      return false;
    }

    /**
     setter for the long form.
     */
    bool set(int& argc, char* argv[], const char* str, const char* def = 0)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return true;
        }

      return false;
    }

    /**
     Extracts the next parameter.
     */
    bool set(int& argc, char* argv[])
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      if (argc > 1)
      {
        m_value = argv[1];
        m_isset = true;
        for (int i = 1; i < argc - 1; ++i)
          argv[i] = argv[i + 1];
        argc -= 1;
        argv[argc] = 0;
        return true;
      }
      else
        return false;
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

////////////////////////////////////////////////////////////////////////
/**
 Special handling for "std::string".

 input-operator for std::string reads just the first word. This is
 not, what we normally expect, so this is speialized here.
 */
template <>
class Arg<std::string>
{
  public:
    Arg(const std::string& def = std::string())
      : m_value(def),
        m_isset(false)
    { }

    /**
     Constructor for the short form.
     */
    Arg(int& argc, char* argv[], char ch, const std::string& def = std::string())
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, ch);
    }

    /**
     Constructor for the long form.
     */
    Arg(int& argc, char* argv[], const char* str, const std::string& def = std::string())
      : m_value(def),
        m_isset(false)
    {
      m_isset = set(argc, argv, str);
    }

    /**
     Extracts the next parameter.
     */
    Arg(int& argc, char* argv[])
      : m_value(std::string()),
        m_isset(false)
    {
      m_isset = set(argc, argv);
    }

    /**
     setter for the short form.
     */
    bool set(int& argc, char* argv[], char ch)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0')
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return true;
        }

      return false;
    }

    /**
     setter for the long form.
     */
    bool set(int& argc, char* argv[], const char* str)
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return true;
        }

      return false;
    }

    /**
     Extracts the next parameter.
     */
    bool set(int& argc, char* argv[])
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      if (argc > 1)
      {
        m_value = argv[1];
        m_isset = true;
        for (int i = 1; i < argc - 1; ++i)
          argv[i] = argv[i + 1];
        argc -= 1;
        argv[argc] = 0;
        return true;
      }
      else
        return false;
    }

    /**
     Extracts the next non-option-parameter.
     */
    bool setNoOpt(int& argc, char* argv[])
    {
      // don't extract value, when already found
      if (m_isset)
        return false;

      for (int i = 1; i < argc; ++i)
        if (argv[i][0] != '-')
        {
          m_isset = true;
          m_value = argv[i];
          for ( ; i < argc - 1; ++i)
            argv[i] = argv[i + 1];
          --argc;
          argv[argc] = 0;
          return true;
        }

      return false;
    }

    /**
     returns the extracted value.
     */
    const std::string& getValue() const   { return m_value; }

    /**
     argument is convertible to "const std::string&"
     */
    operator const std::string&() const   { return m_value; }

    /**
     returns true, when the option is not set and the default is used.
     */
    bool isSet() const             { return m_isset; }

  private:
    std::string m_value;
    bool        m_isset;
};

}

#endif // CXXTOOLS_ARG_H
