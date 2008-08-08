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
  {
    log_warn("attached thread not joined");
    pthread_join(pthreadId, 0);
  }
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

}
