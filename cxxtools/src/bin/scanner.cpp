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

#include "scanner.h"
#include <cxxtools/log.h>
#include <cxxtools/remoteexception.h>

log_define("cxxtools.bin.scanner")

namespace cxxtools
{
namespace bin
{

bool Scanner::advance(char ch)
{
    log_debug("process " << static_cast<unsigned>(static_cast<unsigned char>(ch)) << " state=" << static_cast<int>(_state));

    switch (_state)
    {
        case state_0:
            if (ch == '\x41')
            {
                _failed = false;
                _state = state_value;
            }
            else if (ch == '\x42')
            {
                _failed = true;
                _state = state_errorcode;
                _count = 4;
            }
            else
                throw std::runtime_error("response expected");
            break;

        case state_value:
            if (_vp.advance(ch))
            {
                _vp.current()->fixup();
                _state = state_end;
            }
            break;

        case state_errorcode:
            _errorCode = (_errorCode << 8) | ch;
            if (--_count == 0)
                _state = state_errormessage;
            break;

        case state_errormessage:
            if (ch == '\0')
                _state = state_end;
            else
                _errorMessage += ch;
            break;

        case state_end:
            if (ch == '\xff')
            {
                log_debug("reply finished");
                return true;
            }
            else
                throw std::runtime_error("end of response marker expected");
            break;
    }

    return false;
}

void Scanner::checkException()
{
    if (_failed)
        throw RemoteException(_errorMessage, _errorCode);
}

}
}
