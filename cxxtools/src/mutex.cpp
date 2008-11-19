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

RWMutex::RWMutex()
{
  int ret = pthread_rwlock_init(&m_rwlock, 0);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_init");
}

RWMutex::~RWMutex()
{
  pthread_rwlock_destroy(&m_rwlock);
}

void RWMutex::rdLock()
{
  int ret = pthread_rwlock_rdlock(&m_rwlock);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_rdlock");
}

void RWMutex::wrLock()
{
  int ret = pthread_rwlock_wrlock(&m_rwlock);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_wrlock");
}

void RWMutex::unlock()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    throw MutexException(ret, "pthread_rwlock_unlock");
}

bool RWMutex::unlockNoThrow()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    log_fatal("cannot unlock rwmutex");
  return ret == 0;
}

Condition::Condition()
{
  int ret = pthread_cond_init(&cond, 0);
  if (ret != 0)
    throw MutexException(ret, "pthread_cond_init");
}

Condition::~Condition()
{
  pthread_cond_destroy(&cond);
}

void Condition::signal()
{
  int ret = pthread_cond_signal(&cond);
  if (ret != 0)
    throw MutexException(ret, "pthread_cond_signal");
}

void Condition::broadcast()
{
  int ret = pthread_cond_broadcast(&cond);
  if (ret != 0)
    throw MutexException(ret, "pthread_cond_broadcast");
}

void Condition::wait(Mutex& mtx)
{
  int ret = pthread_cond_wait(&cond, &mtx.m_mutex);
  if (ret != 0)
    throw MutexException(ret, "pthread_cond_wait");
}

bool Condition::timedwait(MutexLock& lock, const struct timespec& time)
{
  // in the timespec structure is expected a relative interval,
  // but pthread_cond_timedwait expects an absolute time.
  struct timeval tp;
  int ret = gettimeofday(&tp, NULL);
  if( ret != 0)
      throw MutexException(ret, "pthread_cond_timedwait");

  // "shift" the timespec structure to the absolute time
  struct timespec absTime = time;
  absTime.tv_sec += tp.tv_sec;
  absTime.tv_nsec += tp.tv_usec*1000;
    
  ret = pthread_cond_timedwait(&cond, &lock.getMutex().m_mutex, &absTime);
  if (ret == ETIMEDOUT)
    return false;

  if (ret != 0)
    throw MutexException(ret, "pthread_cond_timedwait");

  return true;
}

bool Condition::wait(MutexLock& lock, unsigned ms)
{
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = ms % 1000 * 1000 * 1000;
  return timedwait(lock, ts);
}

}
