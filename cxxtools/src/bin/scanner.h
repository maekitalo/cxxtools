/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef CXXTOOLS_BIN_SCANNER_H
#define CXXTOOLS_BIN_SCANNER_H

#include <cxxtools/deserializer.h>
#include <cxxtools/bin/valueparser.h>
#include <string>

namespace cxxtools
{
    namespace bin
    {
        class Scanner
        {
            public:
                Scanner()
                    : _state(state_0),
                      _failed(false)
                { }

                void begin(IDeserializer* deserializer)
                {
                    _vp.begin(*deserializer, _context);
                    _state = state_0;
                    _failed = false;
                    _errorCode = 0;
                    _errorMessage.clear();
                }

                bool advance(char ch);

            private:
                enum
                {
                    state_0,
                    state_value,
                    state_errorcode,
                    state_errormessage,
                    state_end,
                } _state;

                ValueParser _vp;
                DeserializationContext _context;
                unsigned short _count;

                bool _failed;
                int _errorCode;
                std::string _errorMessage;
        };
    }
}

#endif // CXXTOOLS_BIN_SCANNER_H
