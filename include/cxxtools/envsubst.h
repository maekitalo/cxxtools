/*
 * Copyright (C) 2015 Tommi Maekitalo
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
 */

#ifndef CXXTOOLS_ENVSUBST_H
#define CXXTOOLS_ENVSUBST_H

#include <stdexcept>
#include <string>

namespace cxxtools
{
  class EnvSubstSyntaxError : public std::runtime_error
  {
    public:
      explicit EnvSubstSyntaxError(const std::string& msg);
  };

  /**
     This function parses a character data and replaces environment variables.

     These replacements are done:

      - A sequence of '$' followed by a sequence of alphanumeric characters or
        underscore is replaced with the content of the environment variable.
        When the variable is not set, it is replaced by a empty sequence.

      - The sequence of alphanumeric characters or underscore can be bracketed
        with curly brackets.

      - In curly brackets a default value can be specified by using the sequence
        ":-". The characters before that are the environment variable and after
        it the replacement. Environment variables in the replacement are
        processed with the same rules.

     Examples:
       The environment variable USER is set to "tommi"

       String sequence                   output
       --------------------------------- --------------------------------
       The user $USER                    The user tommi
       The user ${USER}                  The user tommi
       The user ${USER:-whoever}         The user tommi
       The user ${UNKNOWN_VAR:-whoever}  The user whoever

   */
  std::string envSubst(const std::string& str);
}

#endif // CXXTOOLS_ENVSUBST_H

