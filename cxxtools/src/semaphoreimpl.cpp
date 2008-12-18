/*
 * Copyright (C) 2005 by Marc Boris Duerner
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
#include "semaphoreimpl.h"
#include "cxxtools/systemerror.h"
#include <cerrno>

namespace cxxtools {

SemaphoreImpl::SemaphoreImpl(unsigned int initial)
{
    int ret = sem_init(&_handle, 0, initial);
    if( ret == -1 )
        throw SystemError( CXXTOOLS_ERROR_MSG("sem_init failed") );
}


SemaphoreImpl::~SemaphoreImpl()
{
    sem_destroy( &_handle );
}


void SemaphoreImpl::wait()
{
    int ret = sem_wait(&_handle);
    if(ret == -1)
        throw SystemError( CXXTOOLS_ERROR_MSG("sem_wait failed") );
}


bool SemaphoreImpl::tryWait()
{
    int ret = sem_trywait( &_handle );
    if(ret == -1)
    {
        if(errno == EAGAIN)
            return false;

        throw SystemError( CXXTOOLS_ERROR_MSG("sem_trywait failed") );
    }

    return true;
}


void SemaphoreImpl::post()
{
    again:
    if( 0 != sem_post(&_handle) )
    {
        if(errno == EINTR)
            goto again;

        throw SystemError( CXXTOOLS_ERROR_MSG("sem_post failed") );
    }
}

} // namespace cxxtools
