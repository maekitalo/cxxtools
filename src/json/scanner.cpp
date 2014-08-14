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

#include "scanner.h"
#include <cxxtools/log.h>
#include <cxxtools/remoteexception.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/composer.h>

log_define("cxxtools.json.scanner")

namespace cxxtools
{

namespace json
{

void Scanner::begin(JsonDeserializer& handler, IComposer& composer)
{
    _deserializer = &handler;
    _deserializer->begin();
    _composer = &composer;
}

void Scanner::finalizeReply()
{
    const SerializationInfo* s = _deserializer->si().findMember("error");

    if (s && !s->isNull())
    {
        log_debug("remote error detected category=" << s->category() << " type=" << s->typeName());

        std::string msg;
        if (s->category() == SerializationInfo::Object)
        {
            int rc = 0;
            s->getMember("code", rc);
            s->getMember("message", msg);
            throw RemoteException(msg, rc);
        }
        else
        {
            s->getValue(msg);
            if (msg.empty())
                msg = "remote exception";

            throw RemoteException(msg);
        }
    }

    _composer->fixup(
        _deserializer->si().getMember("result"));
}

}
}
