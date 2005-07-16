/* thread.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/thread.h"
#include <errno.h>
#include <sstream>

namespace cxxtools
{

std::string ThreadException::formatMsg(const char* method, int e)
{
  std::ostringstream msg;
  msg << "error " << e << " in " << method;
  return msg.str();
}

Thread::Thread()
{
  pthread_attr_init(&pthread_attr);
}

Thread::~Thread()
{
  pthread_attr_destroy(&pthread_attr);
}

void Thread::create()
{
  int ret = pthread_create(&pthread, &pthread_attr, Thread::start, (void*)this);
  if (ret != 0)
    throw ThreadException("pthread_create", ret);
}

void Thread::join()
{
  int ret = pthread_join(pthread, 0);
  if (ret != 0)
    throw ThreadException("pthread_join", ret);
}

void* Thread::start(void* arg)
{
  ((Thread*)arg)->run();
  return 0;
}

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

}
