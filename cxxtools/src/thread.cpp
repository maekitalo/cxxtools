////////////////////////////////////////////////////////////////////////
// thread.cpp
//
// Tommi Mäkitalo
//

#include "cxxtools/thread.h"
#include <errno.h>

Thread::Thread()
{
  pthread_attr_init(&pthread_attr);
}

Thread::~Thread()
{
  pthread_attr_destroy(&pthread_attr);
}

void Thread::Create()
{
  int ret = pthread_create(&pthread, &pthread_attr, Thread::start, (void*)this);
  if (ret != 0)
    throw ThreadException("error in pthread_create", ret);
}

void Thread::Join()
{
  int ret = pthread_join(pthread, 0);
  if (ret != 0)
    throw ThreadException("error in pthread_join", ret);
}

void* Thread::start(void* arg)
{
  ((Thread*)arg)->Run();
  return 0;
}

Mutex::Mutex()
{
  int ret = pthread_mutex_init(&m_mutex, 0);
  if (ret != 0)
    throw ThreadException("error in pthread_mutex_init", ret);
}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&m_mutex);
}

void Mutex::Lock()
{
  int ret = pthread_mutex_lock(&m_mutex);
  if (ret != 0)
    throw ThreadException("error in pthread_mutex_lock", ret);
}

bool Mutex::tryLock()
{
  int ret = pthread_mutex_trylock(&m_mutex);
  if (ret == 0)
    return true;
  else
    return false;
}

void Mutex::Unlock()
{
  int ret = pthread_mutex_unlock(&m_mutex);
  if (ret != 0)
    throw ThreadException("error in pthread_mutex_unlock", ret);
}

RWLock::RWLock()
{
  int ret = pthread_rwlock_init(&m_rwlock, 0);
  if (ret != 0)
    throw ThreadException("error in pthread_rwlock_init", ret);
}

RWLock::~RWLock()
{
  pthread_rwlock_destroy(&m_rwlock);
}

void RWLock::RdLock()
{
  int ret = pthread_rwlock_rdlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException("error in pthread_rwlock_rdlock", ret);
}

void RWLock::WrLock()
{
  int ret = pthread_rwlock_wrlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException("error in pthread_rwlock_wrlock", ret);
}

void RWLock::Unlock()
{
  int ret = pthread_rwlock_unlock(&m_rwlock);
  if (ret != 0)
    throw ThreadException("error in pthread_rwlock_unlock", ret);
}

Semaphore::Semaphore(unsigned value)
{
  int ret = sem_init(&sem, 0, value);
  if (ret != 0)
    throw ThreadException("error in sem_init", ret);
}

Semaphore::~Semaphore()
{
  sem_destroy(&sem);
}

void Semaphore::Wait()
{
  int ret = sem_wait(&sem);
  if (ret != 0)
    throw ThreadException("error in sem_wait", ret);
}

bool Semaphore::TryWait()
{
  int ret = sem_trywait(&sem);
  if (ret != EAGAIN && ret != 0)
    throw ThreadException("error in sem_trywait", ret);
  return ret == 0;
}

void Semaphore::Post()
{
  int ret = sem_post(&sem);
  if (ret != 0)
    throw ThreadException("error in sem_post", ret);
}

int Semaphore::getValue()
{
  int sval;
  int ret = sem_getvalue(&sem, &sval);
  if (ret != 0)
    throw ThreadException("error in sem_getvalue", ret);
  return sval;
}

Condition::Condition()
{
  int ret = pthread_cond_init(&cond, 0);
  if (ret != 0)
    throw ThreadException("error in pthread_cond_init", ret);
}

void Condition::Signal()
{
  int ret = pthread_cond_signal(&cond);
  if (ret != 0)
    throw ThreadException("error in pthread_cond_signal", ret);
}

void Condition::Broadcast()
{
  int ret = pthread_cond_broadcast(&cond);
  if (ret != 0)
    throw ThreadException("error in pthread_cond_broadcast", ret);
}

void Condition::Wait(ConditionLock& lock)
{
  int ret = pthread_cond_wait(&cond, &m_mutex);
  if (ret != 0)
    throw ThreadException("error in pthread_cond_wait", ret);
}

