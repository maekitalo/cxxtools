/*
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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

#include "cxxtools/mutex.h"
#include "cxxtools/log.h"
#include <sys/time.h>
#include <errno.h>
#include <string>

log_define("cxxtools.mutex")

namespace cxxtools
{

//
// Mutex
//

Mutex::Mutex()
{
  int ret = pthread_mutex_init(&m_mutex, 0);
  if (ret != 0)
    throw MutexException(ret, "pthread_mutex_init");
}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
  int ret = pthread_mutex_lock(&m_mutex);
  if (ret != 0)
    throw MutexException(ret, "pthread_mutex_lock");
}

bool Mutex::tryLock()
{
  int ret = pthread_mutex_trylock(&m_mutex);
  if (ret == EBUSY)
    return false;
  else if (ret == 0)
    return true;
  else
    throw MutexException(ret, "pthread_mutex_trylock");
}

void Mutex::unlock()
{
  int ret = pthread_mutex_unlock(&m_mutex);
  if (ret != 0)
    throw MutexException(ret, "pthread_mutex_unlock");
}

bool Mutex::unlockNoThrow()
{
  int ret = pthread_mutex_unlock(&m_mutex);
  if (ret != 0)
    log_fatal("cannot unlock mutex");
  return ret == 0;
}

ReadWriteMutex::ReadWriteMutex()
{
  int ret = pthread_rwlock_init(&m_rwlock, 0);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_init");
}

ReadWriteMutex::~ReadWriteMutex()
{
  pthread_rwlock_destroy(&m_rwlock);
}

void ReadWriteMutex::readLock()
{
  int ret = pthread_rwlock_rdlock(&m_rwlock);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_rdlock");
}

bool ReadWriteMutex::tryReadLock()
{
    int rc = pthread_rwlock_tryrdlock(&m_rwlock);

    if( rc != 0 && rc != EBUSY)
      throw MutexException(rc, "pthread_rwlock_tryrdlock");

    return rc != EBUSY;
}

void ReadWriteMutex::writeLock()
{
  int ret = pthread_rwlock_wrlock(&m_rwlock);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_wrlock");
}

bool ReadWriteMutex::tryWriteLock()
{
    int rc = pthread_rwlock_trywrlock(&m_rwlock);

    if( rc != 0 && rc != EBUSY)
      throw MutexException(rc, "pthread_rwlock_trywrlock");

    return rc != EBUSY;
}

void ReadWriteMutex::unlock()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_unlock");
}

bool ReadWriteMutex::unlockNoThrow()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    log_fatal("cannot unlock rwmutex");
  return ret == 0;
}

}
