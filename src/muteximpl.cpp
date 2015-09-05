/*
 * Copyright (C) 2005 - 2007 by Marc Boris Duerner
 * Copyright (C) 2005 - 2007 by Aloysius Indrayanto
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
#include "cxxtools/systemerror.h"
#include <errno.h>

namespace cxxtools {

MutexImpl::MutexImpl()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    int rc = pthread_mutex_init(&_handle, &attr);
    if (rc != 0)
        throw SystemError(rc, "pthread_mutex_init");
}


MutexImpl::MutexImpl(bool recursive)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    if (recursive)
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    int rc = pthread_mutex_init(&_handle, &attr);
    if (rc != 0)
        throw SystemError(rc, "pthread_mutex_init");
}


MutexImpl::~MutexImpl()
{
    pthread_mutex_destroy(&_handle);
}


void MutexImpl::lock()
{
   int rc = pthread_mutex_lock(&_handle);
   if( rc != 0 )
       throw SystemError(rc, "pthread_mutex_lock failed");
}


bool MutexImpl::tryLock()
{
    int rc = pthread_mutex_trylock(&_handle);

    if( rc != 0 && rc != EBUSY )
        throw SystemError(rc, "pthread_mutex_trylock");

    return rc != EBUSY;
}


void MutexImpl::unlock()
{
   int rc = pthread_mutex_unlock(&_handle);
   if( rc != 0 )
       throw SystemError(rc, "pthread_mutex_unlock");
}


ReadWriteMutexImpl::ReadWriteMutexImpl()
{
    int rc = pthread_rwlock_init(&_rwl, NULL);
    if( rc != 0 )
        throw SystemError(rc, "pthread_rwlock_init");
}


ReadWriteMutexImpl::~ReadWriteMutexImpl()
{
    pthread_rwlock_destroy(&_rwl);
}


void ReadWriteMutexImpl::readLock()
{
    int rc = pthread_rwlock_rdlock(&_rwl);
    if( rc != 0 )
        throw SystemError(rc, "pthread_rwlock_rdlock");
}


bool ReadWriteMutexImpl::tryReadLock()
{
    int rc = pthread_rwlock_tryrdlock(&_rwl);

    if( rc != 0 && rc != EBUSY )
        throw SystemError(rc, "pthread_rwlock_tryrdlock");

    return rc != EBUSY;
}


void ReadWriteMutexImpl::writeLock()
{
    int rc = pthread_rwlock_wrlock(&_rwl);
    if( rc != 0)
        throw SystemError(rc, "pthread_rwlock_wrlock");
}


bool ReadWriteMutexImpl::tryWriteLock()
{
    int rc = pthread_rwlock_trywrlock(&_rwl);

    if( rc != 0 && rc != EBUSY)
        throw SystemError(rc, "pthread_rwlock_trywrlock");

    return rc != EBUSY;
}


void ReadWriteMutexImpl::unlock()
{
    int rc = pthread_rwlock_unlock(&_rwl);
    if( rc != 0 )
        throw SystemError(rc, "pthread_rwlock_unlock");
}

} // !namespace cxxtools
