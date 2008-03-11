/* thread.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "cxxtools/thread.h"
#include "cxxtools/log.h"
#include <sys/time.h>
#include <errno.h>
#include <sstream>
#include <string>

log_define("cxxtools.thread")

namespace cxxtools
{

Thread::Thread()
  : pthreadId(0)
{
  pthread_attr_init(&pthread_attr);
}

Thread::~Thread()
{
  pthread_attr_destroy(&pthread_attr);
}

void Thread::kill(int signo)
{
  pthread_kill(pthreadId, signo);
}

//
// AttachedThread
//

AttachedThread::~AttachedThread()
{
  if (!joined && pthreadId)
    pthread_join(pthreadId, 0);
}

void* AttachedThread::start(void* arg)
{
  AttachedThread* t = static_cast<AttachedThread*>(arg);
  try
  {
    t->run();
  }
  catch (...)
  {
  }
  return 0;
}

void AttachedThread::create()
{
  int ret = pthread_create(&pthreadId, &pthread_attr, start, (void*)this);
  if (ret != 0)
    throw ThreadException(ret, "pthread_create");
  joined = false;
}

void AttachedThread::join()
{
  if (!joined && pthreadId != 0)
  {
    int ret = pthread_join(pthreadId, 0);
    if (ret != 0)
      throw ThreadException(ret, "pthread_join");
    joined = true;
  }
}

//
// DetachedThread
//

DetachedThread::DetachedThread()
{
  int ret = pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
  if (ret != 0)
    throw ThreadException(ret, "pthread_attr_setdetachstate");
}

void* DetachedThread::start(void* arg)
{
  DetachedThread* t = static_cast<DetachedThread*>(arg);
  try
  {
    t->run();
  }
  catch (...)
  {
  }
  delete t;
  return 0;
}

void DetachedThread::create()
{
  int ret = pthread_create(&pthreadId, &pthread_attr, start, (void*)this);
  if (ret != 0)
    throw ThreadException(ret, "pthread_create");
}

//
// Mutex
//

Mutex::Mutex()
{
  int ret = pthread_mutex_init(&m_mutex, 0);
  if (ret != 0)
    throw ThreadException(ret, "pthread_mutex_init");
}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
  int ret = pthread_mutex_lock(&m_mutex);
  if (ret != 0)
    throw ThreadException(ret, "pthread_mutex_lock");
}

bool Mutex::tryLock()
{
  int ret = pthread_mutex_trylock(&m_mutex);
  if (ret == EBUSY)
    return false;
  else if (ret == 0)
    return true;
  else
    throw ThreadException(ret, "pthread_mutex_trylock");
}

void Mutex::unlock()
{
  int ret = pthread_mutex_unlock(&m_mutex);
  if (ret != 0)
    throw ThreadException(ret, "pthread_mutex_unlock");
}

bool Mutex::unlockNoThrow()
{
  int ret = pthread_mutex_unlock(&m_mutex);
  if (ret != 0)
    log_fatal("cannot unlock mutex");
  return ret == 0;
}

RWLock::RWLock()
{
  int ret = pthread_rwlock_init(&m_rwlock, 0);
  if (ret != 0)
    throw ThreadException(ret, "pthread_rwlock_init");
}

RWLock::~RWLock()
{
  pthread_rwlock_destroy(&m_rwlock);
}

void RWLock::rdLock()
{
  int ret = pthread_rwlock_rdlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException(ret, "pthread_rwlock_rdlock");
}

void RWLock::wrLock()
{
  int ret = pthread_rwlock_wrlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException(ret, "pthread_rwlock_wrlock");
}

void RWLock::unlock()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException(ret, "pthread_rwlock_unlock");
}

bool RWLock::unlockNoThrow()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    log_fatal("cannot unlock rwmutex");
  return ret == 0;
}

Semaphore::Semaphore(unsigned value)
{
  int ret = sem_init(&sem, 0, value);
  if (ret != 0)
    throw ThreadException(ret, "sem_init");
}

Semaphore::~Semaphore()
{
  sem_destroy(&sem);
}

void Semaphore::wait()
{
  int ret = sem_wait(&sem);
  if (ret != 0)
    throw ThreadException(ret, "sem_wait");
}

bool Semaphore::tryWait()
{
  int ret = sem_trywait(&sem);
  if (ret != EAGAIN && ret != 0)
    throw ThreadException(ret, "sem_trywait");
  return ret == 0;
}

void Semaphore::post()
{
  int ret = sem_post(&sem);
  if (ret != 0)
    throw ThreadException(ret, "sem_post");
}

int Semaphore::getValue()
{
  int sval;
  int ret = sem_getvalue(&sem, &sval);
  if (ret != 0)
    throw ThreadException(ret, "sem_getvalue");
  return sval;
}

Condition::Condition()
{
  int ret = pthread_cond_init(&cond, 0);
  if (ret != 0)
    throw ThreadException(ret, "pthread_cond_init");
}

Condition::~Condition()
{
  pthread_cond_destroy(&cond);
}

void Condition::signal()
{
  int ret = pthread_cond_signal(&cond);
  if (ret != 0)
    throw ThreadException(ret, "pthread_cond_signal");
}

void Condition::broadcast()
{
  int ret = pthread_cond_broadcast(&cond);
  if (ret != 0)
    throw ThreadException(ret, "pthread_cond_broadcast");
}

void Condition::wait(MutexLock& lock)
{
  int ret = pthread_cond_wait(&cond, &lock.getMutex().m_mutex);
  if (ret != 0)
    throw ThreadException(ret, "pthread_cond_wait");
}

bool Condition::timedwait(MutexLock& lock, const struct timespec& time)
{
  // in the timespec structure is expected a relative interval,
  // but pthread_cond_timedwait expects an absolute time.
  struct timeval tp;
  int ret = gettimeofday(&tp, NULL);
  if( ret != 0)
      throw ThreadException(ret, "pthread_cond_timedwait");

  // "shift" the timespec structure to the absolute time
  struct timespec absTime = time;
  absTime.tv_sec += tp.tv_sec;
  absTime.tv_nsec += tp.tv_usec*1000;
    
  ret = pthread_cond_timedwait(&cond, &lock.getMutex().m_mutex, &absTime);
  if (ret == ETIMEDOUT)
    return false;

  if (ret != 0)
    throw ThreadException(ret, "pthread_cond_timedwait");

  return true;
}

bool Condition::timedwait(MutexLock& lock, unsigned ms)
{
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = ms % 1000 * 1000 * 1000;
  return timedwait(lock, ts);
}

}
