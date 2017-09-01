/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_BIN_SCANNER_H
#define CXXTOOLS_BIN_SCANNER_H

#include <cxxtools/composer.h>
#include <cxxtools/bin/parser.h>

#include <string>
#include <iosfwd>

namespace cxxtools
{
    namespace bin
    {
        class Deserializer;

        class Scanner
        {
            public:
                Scanner()
                    : _state(state_0),
                      _deserializer(0),
                      _composer(0),
                      _count(0),
                      _failed(false),
                      _errorCode(0)
                { }

                void begin(Deserializer& handler, IComposer& composer);

                bool advance(std::streambuf& in);

                void finish();

            private:
                enum
                {
                    state_0,
                    state_value,
                    state_errorcode,
                    state_errormessage,
                    state_end
                } _state;

                Parser _vp;
                Deserializer* _deserializer;
                IComposer* _composer;

                unsigned short _count;

                bool _failed;
                int _errorCode;
                std::string _errorMessage;
        };
    }
}

#endif // CXXTOOLS_BIN_SCANNER_H
