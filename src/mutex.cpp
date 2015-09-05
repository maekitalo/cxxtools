/*
 * Copyright (C) 2005-2008 by Marc Boris Duerner
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
#include "muteximpl.h"
#include "cxxtools/mutex.h"

namespace cxxtools {

Mutex::Mutex()
{
    _impl = new MutexImpl();
}


Mutex::~Mutex()
{
    delete _impl;
}


void Mutex::lock()
{
    _impl->lock();
}


bool Mutex::tryLock()
{
    return _impl->tryLock();
}


void Mutex::unlock()
{
    _impl->unlock();
}


bool Mutex::unlockNoThrow()
{
    try
    {
        _impl->unlock();
        return true;
    }
    catch(...)
    {}

    return false;
}


RecursiveMutex::RecursiveMutex()
{
    _impl = new MutexImpl(true);
}


RecursiveMutex::~RecursiveMutex()
{
    delete _impl;
}


void RecursiveMutex::lock()
{
    _impl->lock();
}


bool RecursiveMutex::tryLock()
{
    return _impl->tryLock();
}


void RecursiveMutex::unlock()
{
    _impl->unlock();
}


bool RecursiveMutex::unlockNoThrow()
{
    try
    {
        _impl->unlock();
        return true;
    }
    catch(...)
    {}

    return false;
}


ReadWriteMutex::ReadWriteMutex()
{
    _impl = new ReadWriteMutexImpl();
}


ReadWriteMutex::~ReadWriteMutex()
{
    delete _impl;
}


void ReadWriteMutex::readLock()
{
    _impl->readLock();
}


bool ReadWriteMutex::tryReadLock()
{
    return _impl->tryReadLock();
}


void ReadWriteMutex::writeLock()
{
    _impl->writeLock();
}


bool ReadWriteMutex::tryWriteLock()
{
    return _impl->tryWriteLock();
}


void ReadWriteMutex::unlock()
{
    _impl->unlock();
}

bool ReadWriteMutex::unlockNoThrow()
{
    try
    {
        _impl->unlock();
        return true;
    }
    catch(...)
    {}

    return false;
}

}
