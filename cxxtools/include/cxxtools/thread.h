////////////////////////////////////////////////////////////////////////
// thread.h
//
// Tommi Mäkitalo
//

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdexcept>

class ThreadException : public std::runtime_error
{
    int m_errno;

  public:
    ThreadException(const std::string& w, int e)
      : std::runtime_error(w),
        m_errno(e)
      { }

    int getErrno() const
    { return m_errno; }
};

class Thread
{
    pthread_t      pthread;
    pthread_attr_t pthread_attr;

  public:
    Thread();
    virtual ~Thread();

    void Create();
    void Join();

    virtual void Run() = 0;

  private:
};

template <typename function_type>
class FunctionThread : public Thread
{
    function_type& function;

  public:
    FunctionThread(function_type& f)
      : function(f)
      { }

    virtual void Run()
    {
      f();
    }
};

template <typename object_type>
class MethodThread : public Thread
{
    object_type& object;
    void (object_type::*method)();

  public:
    MethodThread(object_type& a, void (object_type::*m)())
      : object(a),
        method(m)
      { }

    virtual void Run()
    {
      (object.*method)();
    }
};

class Mutex
{
  protected:
    pthread_mutex_t m_mutex;

    // make copy and assignment private without implementation
    Mutex(const Mutex&);
    const Mutex& operator= (const Mutex&);

  public:
    Mutex();
    ~Mutex();

    void Lock();
    bool tryLock();
    void Unlock();
};

class RWLock
{
    pthread_rwlock_t m_rwlock;

    // make copy and assignment private without implementation
    RWLock(const RWLock&);
    const RWLock& operator= (const RWLock&);

  public:
    RWLock();
    ~RWLock();

    void RdLock();
    void WrLock();
    void Unlock();
};

template <class mutex_type,
          void (mutex_type::*lock_method)() = &mutex_type::Lock,
          void (mutex_type::*unlock_method)() = &mutex_type::Unlock>
class LockBase
{
    mutex_type& mutex;
    bool locked;

    // make copy and assignment private without implementation
    LockBase(const LockBase&);
    const LockBase& operator= (const LockBase&);

  public:
    LockBase(mutex_type& m, bool lock = true)
      : mutex(m), locked(false)
    {
      if (lock)
        Lock();
    }

    ~LockBase()
    {
      if (locked)
        Unlock();
    }

    void Lock()
    {
      if (!locked)
      {
        (mutex.*lock_method)();
        locked = true;
      }
    }

    void Unlock()
    {
      if (locked)
      {
        (mutex.*unlock_method)();
        locked = false;
      }
    }

    mutex_type& getMutex()
      { return mutex; }
};

typedef LockBase<Mutex> MutexLock;
typedef LockBase<RWLock, &RWLock::RdLock> RdLock;
typedef LockBase<RWLock, &RWLock::WrLock> WrLock;
typedef LockBase<Mutex> ConditionLock;

class Semaphore
{
    sem_t sem;

  public:
    Semaphore(unsigned value);
    ~Semaphore();

    void Wait();
    bool TryWait();
    void Post();
    int getValue();
};

class Condition : public Mutex
{
    pthread_cond_t cond;

    // no implementation
    Condition(const Condition&);
    const Condition& operator= (const Condition&);

  public:
    Condition();

    void Signal();
    void Broadcast();
    void Wait(ConditionLock& lock);
};

#endif // THREAD_H

