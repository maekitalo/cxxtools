/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#include "settingsreader.h"

namespace cxxtools
{

void SettingsReader::State::syntaxError(unsigned line)
{
    throw SettingsError("syntax error", line);
}


void SettingsReader::parse(cxxtools::SerializationInfo& si)
{
    while (!_current.empty())
        _current.pop();

    _current.push(&si);
    state = BeginStatement::instance();
    _line  = 1;
    _isDotted = false;
    Char ch = Char(0);

    while ( _is->get(ch) )
    {
        state = state->onChar(ch, *this);

        if(ch == '\n')
        {
            ++_line;
        }
    }

    // if exceptions are deactivated caller must check
    // istream for failure
    if( _is->bad() )
        return;

    state->onEof(*this);
}


void SettingsReader::enterMember()
{
    //
    // Consider namespace at top-level. For example a.b.c means c
    // as a child of a.b. both are only added when not present.
    // If we are not top-level, always add a node.
    //
    if( _depth == 0 )
    {
        std::string name;
        if( _section.size() )
        {
            name += _section.narrow();
            name += '.';
            name += _token.narrow();
        }
        else
        {
            name = _token.narrow();
        }

        //
        // Add a serialization node for the parent if not present.
        // In this example the parent is a.b
        //
        size_t pos = name.rfind('.');
        if(pos != std::string::npos)
        {
            std::string root = name.substr( 0, pos );
            cxxtools::SerializationInfo* c = current()->findMember( root );
            if(c == 0)
                c = &( current()->addMember( root ) );

            _current.push(c);
            ++_depth;

            _isDotted = true; // remember that we have to leave twice later
            name = name.substr( ++pos ); // TODO: use remove or erase
        }

        //
        // Add a node for the actual value if not present. In this
        // example c is a parent of a.b
        //
        cxxtools::SerializationInfo* c = current()->findMember( name );
        if(c == 0)
            c = &( current()->addMember( name ) );

        _current.push(c);
    }
    else
    {
        _current.push( &( current()->addMember( _token.narrow() ) ) );
    }

    ++_depth;
    _token.clear();
}


void SettingsReader::leaveMember()
{
    //std::cerr << "@" << std::endl;

    if (_current.empty())
        throw SettingsError("too many closing braces", _line);

    _current.pop();
    --_depth;

    if(_depth == 1 && _isDotted)
    {
        // leaving a dotted entry
        _current.pop();
        _isDotted = false;
        --_depth;
    }
}


void SettingsReader::pushValue()
{
    current()->setValue(std::move(_token));
    _token.clear();
}


void SettingsReader::pushTypeName()
{
    current()->setTypeName( _token.narrow() );
    _token.clear();
}


void SettingsReader::pushName()
{
    current()->setName( _token.narrow() );
    _token.clear();
}


cxxtools::Char SettingsReader::getEscaped()
{
    cxxtools::Char ch;
    if( ! _is->get(ch) )
        throw SettingsError("unexpected EOF", _line);

    switch( ch.value() )
    {
        case 'n':
            return cxxtools::Char(L'\n');

        case 'r':
            return cxxtools::Char(L'\r');
    }

    return ch;
}

}
