/*
 * Copyright (C) 2024 by Tommi Maekitalo
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

#ifndef CXXTOOLS_DESTRUCTIONSENTRY_H
#define CXXTOOLS_DESTRUCTIONSENTRY_H

namespace cxxtools
{
class DestructionSentryPtr;

/* A destruction sentry monitors deletion of a object.
 *
 * When a member function results in a deletion of the object then the function
 * must not access member data any more. Sometimes the deletion is a result of
 * a callback and hence hidden. This desctruction sentry helps detecting, that
 * the object is deleted.
 *
 * To use it:
 *  * a DestructionSentryPtr must be a member of the object
 *  * In the function, which needs to check deletion create a DescrutionSentry
 *    on the stack and pass the DestructionSentryPtr to the constructor.
 *  * use the member function `deleted()`from the sentry object after
 *    a possible deletion before accessing member data.
 */
class DestructionSentry
{
public:
    explicit DestructionSentry(DestructionSentry*& sentry)
        : _deleted(false),
          _sentry(sentry)
    {
       sentry = this;
    }

    explicit DestructionSentry(DestructionSentryPtr& ptr);

    ~DestructionSentry()
    {
        if( ! _deleted )
            this->detach();
    }

    bool operator!() const
    { return _deleted; }

    bool deleted() const
    { return _deleted; }

    void detach()
    {
        _sentry = 0;
        _deleted = true;
    }

private:
    bool _deleted;
    DestructionSentry*& _sentry;
};

class DestructionSentryPtr
{
    friend class DestructionSentry;
    DestructionSentry* _ptr = nullptr;

public:
    DestructionSentryPtr() = default;
    ~DestructionSentryPtr()
    {
        if (_ptr)
            _ptr->detach();
    }
};

inline DestructionSentry::DestructionSentry(DestructionSentryPtr& ptr)
    : _deleted(false),
      _sentry(ptr._ptr)
{
    ptr._ptr = this;
}

}

#endif
