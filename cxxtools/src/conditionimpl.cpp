/***************************************************************************
 *   Copyright (C) 2005 - 2007 by Marc Boris Durrner                       *
 *   Copyright (C) 2005 - 2007 by Sebastian Pieck                          *
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
#include "conditionimpl.h"
#include "muteximpl.h"
#include "cxxtools/systemerror.h"
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

namespace cxxtools {

ConditionImpl::ConditionImpl()
{
    if ( pthread_cond_init( &_cond, NULL ) != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_cond_init failed") );
}


ConditionImpl::~ConditionImpl()
{
    pthread_cond_destroy(&_cond);
}


void ConditionImpl::wait(Mutex& mtx)
{
    if ( pthread_cond_wait(&_cond, mtx.impl().handle() ) != 0)
       throw SystemError( CXXTOOLS_ERROR_MSG("pthread_cond_wait failed") );
}


bool ConditionImpl::wait(Mutex& mtx, unsigned int ms)
{
    int result;

    struct timeval tv;
    ::gettimeofday(&tv, NULL);

    struct timespec ts;
    ts.tv_nsec = ((ms%1000) * 1000 + tv.tv_usec) * 1000;
    ts.tv_sec = (ms/1000) + (ts.tv_nsec/1000000000) + tv.tv_sec;
    ts.tv_nsec = ts.tv_nsec % 1000000000;

    do
    {
        result = pthread_cond_timedwait(&_cond, mtx.impl().handle(), &ts);
    }
    while(result == EINTR);

    if(result != 0 && result != ETIMEDOUT)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_cond_timedwait failed") );

    return result != ETIMEDOUT;
}


void ConditionImpl::signal()
{
    if( pthread_cond_signal( &_cond ) != 0 )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_cond_signal failed") );
}


void ConditionImpl::broadcast()
{
    if( pthread_cond_broadcast( &_cond ) != 0 )
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_cond_broadcast failed") );
}

}
