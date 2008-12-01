/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
    _impl = new MutexImpl(1);
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
