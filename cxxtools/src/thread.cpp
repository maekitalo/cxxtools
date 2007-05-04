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
#include <errno.h>
#include <sstream>
#include <string>

log_define("cxxtools.thread");

namespace cxxtools
{

std::string ThreadException::formatMsg(const char* method, int e)
{
  std::ostringstream msg;
  msg << "error " << e << " in " << method;
  return msg.str();
}

Thread::Thread()
  : pthreadId(0)
{
  pthread_attr_init(&pthread_attr);
}

Thread::~Thread()
{
  pthread_attr_destroy(&pthread_attr);
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
    throw ThreadException("pthread_create", ret);
}

void AttachedThread::join()
{
  if (!joined && pthreadId != 0)
  {
    int ret = pthread_join(pthreadId, 0);
    if (ret != 0)
      throw ThreadException("pthread_join", ret);
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
    throw ThreadException("pthread_attr_setdetachstate", ret);
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
    throw ThreadException("pthread_create", ret);
}

//
// Mutex
//

Mutex::Mutex()
{
  int ret = pthread_mutex_init(&m_mutex, 0);
  if (ret != 0)
    throw ThreadException("pthread_mutex_init", ret);
}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
  int ret = pthread_mutex_lock(&m_mutex);
  if (ret != 0)
    throw ThreadException("pthread_mutex_lock", ret);
}

bool Mutex::tryLock()
{
  int ret = pthread_mutex_trylock(&m_mutex);
  if (ret == 0)
    return true;
  else
    return false;
}

void Mutex::unlock()
{
  int ret = pthread_mutex_unlock(&m_mutex);
  if (ret != 0)
    throw ThreadException("pthread_mutex_unlock", ret);
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
    throw ThreadException("pthread_rwlock_init", ret);
}

RWLock::~RWLock()
{
  pthread_rwlock_destroy(&m_rwlock);
}

void RWLock::rdLock()
{
  int ret = pthread_rwlock_rdlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException("pthread_rwlock_rdlock", ret);
}

void RWLock::wrLock()
{
  int ret = pthread_rwlock_wrlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException("pthread_rwlock_wrlock", ret);
}

void RWLock::unlock()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException("pthread_rwlock_unlock", ret);
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
    throw ThreadException("sem_init", ret);
}

Semaphore::~Semaphore()
{
  sem_destroy(&sem);
}

void Semaphore::wait()
{
  int ret = sem_wait(&sem);
  if (ret != 0)
    throw ThreadException("sem_wait", ret);
}

bool Semaphore::tryWait()
{
  int ret = sem_trywait(&sem);
  if (ret != EAGAIN && ret != 0)
    throw ThreadException("sem_trywait", ret);
  return ret == 0;
}

void Semaphore::post()
{
  int ret = sem_post(&sem);
  if (ret != 0)
    throw ThreadException("sem_post", ret);
}

int Semaphore::getValue()
{
  int sval;
  int ret = sem_getvalue(&sem, &sval);
  if (ret != 0)
    throw ThreadException("sem_getvalue", ret);
  return sval;
}

Condition::Condition()
{
  int ret = pthread_cond_init(&cond, 0);
  if (ret != 0)
    throw ThreadException("pthread_cond_init", ret);
}

Condition::~Condition()
{
  pthread_cond_destroy(&cond);
}

void Condition::signal()
{
  int ret = pthread_cond_signal(&cond);
  if (ret != 0)
    throw ThreadException("pthread_cond_signal", ret);
}

void Condition::broadcast()
{
  int ret = pthread_cond_broadcast(&cond);
  if (ret != 0)
    throw ThreadException("pthread_cond_broadcast", ret);
}

void Condition::wait(MutexLock& lock)
{
  int ret = pthread_cond_wait(&cond, &lock.getMutex().m_mutex);
  if (ret != 0)
    throw ThreadException("pthread_cond_wait", ret);
}

bool Condition::timedwait(MutexLock& lock, const struct timespec& time)
{
  int ret = pthread_cond_timedwait(&cond, &lock.getMutex().m_mutex, &time);
  if (ret == ETIMEDOUT)
    return false;

  if (ret != 0)
    throw ThreadException("pthread_cond_timedwait", ret);

  return true;
}

bool Condition::timedwait(MutexLock& lock, unsigned ms)
{
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = ms * 1000 % 1000;
  return timedwait(lock, ts);
}

}
