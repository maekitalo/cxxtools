/*
 * Copyright (C) 2005 - 2007 by Marc Boris Durrner
 * Copyright (C) 2005 - 2007 by Sebastian Pieck
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
#include "conditionimpl.h"
#include "muteximpl.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/timespan.h"
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

namespace cxxtools {

ConditionImpl::ConditionImpl()
{
    int rc = pthread_cond_init( &_cond, NULL );
    if ( rc != 0)
        throw SystemError( rc, "pthread_cond_init failed");
}


ConditionImpl::~ConditionImpl()
{
    pthread_cond_destroy(&_cond);
}


void ConditionImpl::wait(Mutex& mtx)
{
    int rc = pthread_cond_wait(&_cond, mtx.impl().handle() );
    if ( rc != 0)
       throw SystemError( rc, "pthread_cond_wait failed");
}


bool ConditionImpl::wait(Mutex& mtx, const Timespan& ts)
{
    if (ts < Timespan(0))
    {
        wait(mtx);
        return true;
    }

    int result;

    Timespan tt = Timespan::gettimeofday() + ts;

    struct timespec tv;

    tv.tv_sec = tt.totalUSecs() / 1000000;
    tv.tv_nsec = (tt.totalUSecs() % 1000000) * 1000;

    do
    {
        result = pthread_cond_timedwait(&_cond, mtx.impl().handle(), &tv);
    }
    while(result == EINTR);

    if(result != 0 && result != ETIMEDOUT)
        throw SystemError( result, "pthread_cond_timedwait failed");

    return result != ETIMEDOUT;
}


void ConditionImpl::signal()
{
    int rc = pthread_cond_signal( &_cond );
    if( rc != 0 )
        throw SystemError( rc, "pthread_cond_signal failed");
}


void ConditionImpl::broadcast()
{
    int rc = pthread_cond_broadcast( &_cond );
    if( rc != 0 )
        throw SystemError( rc, "pthread_cond_broadcast failed");
}

}
