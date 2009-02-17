/*
 * Copyright (C) 2008 Marc Boris Duerner
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

#include "cxxtools/selectable.h"

namespace cxxtools {

Selectable::~Selectable()
{
}


void Selectable::setSelector(SelectorBase* parent)
{
    if(_parent)
    {
        this->onDetach(*_parent);

        if( this->enabled() )
            _parent->onRemove(*this);

        _parent = 0;
    }

    if(parent)
    {
        this->onAttach(*parent);

        if( this->enabled() )
            parent->onAdd(*this);
    }

    _parent = parent;
}


SelectorBase* Selectable::selector()
{
    return _parent;
}


const SelectorBase* Selectable::selector() const
{
    return _parent;
}


void Selectable::close()
{
    if( this->enabled() )
    {
        this->setEnabled(false);
        this->onClose();
    }
}

bool Selectable::wait(std::size_t msecs)
{
    return this->onWait(msecs); 
}


bool Selectable::enabled() const
{
    return _state != Disabled;
}


bool Selectable::idle() const
{
    return _state == Idle;
}


bool Selectable::busy() const
{
    return _state == Busy;
}


bool Selectable::avail() const
{
    return _state == Avail;
}


Selectable::Selectable()
: _parent(0)
, _state(Disabled)
{ }


void Selectable::setEnabled(bool isEnabled)
{
    if(isEnabled)
    {
        if(_state == Disabled)
            this->setState(Idle);
        else
        {
            this->setState(_state);
            // if(_parent) _parent->onReinit(*this)
        }
    }
    else
    {
        this->setState(Disabled);
    }
}


void Selectable::setState(State state)
{
    if(state == Disabled)
    {
        if(_parent)
            _parent->onRemove(*this);
    }
    else if(_state == Disabled)
    {
        if(_parent)
            _parent->onAdd(*this);
    }

    //State prev = _state;
    _state = state;

    if(_parent)
    {
        _parent->onChanged(*this /*, prev */);
    }
}

}
