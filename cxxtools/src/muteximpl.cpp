/***************************************************************************
 *   Copyright (C) 2005 - 2007 by Marc Boris Duerner                       *
 *   Copyright (C) 2005 - 2007 by Aloysius Indrayanto                      *
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
#include "cxxtools/systemerror.h"
#include <errno.h>

namespace cxxtools {

MutexImpl::MutexImpl()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK  );

    int ret = pthread_mutex_init(&_handle, &attr);
    if (ret != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_mutex_init failed") );
}


MutexImpl::MutexImpl(int recursive)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );

    int ret = pthread_mutex_init(&_handle, &attr);
    if (ret != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_mutex_init failed") );
}


MutexImpl::~MutexImpl()
{
    pthread_mutex_destroy(&_handle);
}


void MutexImpl::lock()
{
   if( pthread_mutex_lock(&_handle) != 0 )
       throw SystemError( CXXTOOLS_ERROR_MSG("pthread_mutex_lock failed") );
}


bool MutexImpl::tryLock()
{
    int rc = pthread_mutex_trylock(&_handle);

    if( rc != 0 && rc != EBUSY )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_mutex_trylock failed") );

    return rc != EBUSY;
}


void MutexImpl::unlock()
{
   if( pthread_mutex_unlock(&_handle) != 0 )
       throw SystemError( CXXTOOLS_ERROR_MSG("pthread_mutex_unlock failed") );
}


ReadWriteMutexImpl::ReadWriteMutexImpl()
{
    if( pthread_rwlock_init(&_rwl, NULL) )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_rwlock_init failed") );
}


ReadWriteMutexImpl::~ReadWriteMutexImpl()
{
    pthread_rwlock_destroy(&_rwl);
}


void ReadWriteMutexImpl::readLock()
{
    if( pthread_rwlock_rdlock(&_rwl) != 0 )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_rwlock_rdlock failed") );
}


bool ReadWriteMutexImpl::tryReadLock()
{
    int rc = pthread_rwlock_tryrdlock(&_rwl);

    if( rc != 0 && rc != EBUSY )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_rwlock_tryrdlock failed") );

    return rc != EBUSY;
}


void ReadWriteMutexImpl::writeLock()
{
    if( pthread_rwlock_wrlock(&_rwl) != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_rwlock_wrlock failed") );
}


bool ReadWriteMutexImpl::tryWriteLock()
{
    int rc = pthread_rwlock_trywrlock(&_rwl);

    if( rc != 0 && rc != EBUSY)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_rwlock_trywrlock failed") );

    return rc != EBUSY;
}


void ReadWriteMutexImpl::unlock()
{
    if( pthread_rwlock_unlock(&_rwl) != 0 )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_rwlock_unlock failed") );
}

} // !namespace cxxtools
