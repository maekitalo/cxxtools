/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#include "threadimpl.h"
#include "cxxtools/systemerror.h"
#include <errno.h>
#include <signal.h>
#include <iostream>

extern "C"
{
    static void* thread_entry(void* arg)
    {
        cxxtools::ThreadImpl* impl = (cxxtools::ThreadImpl*)arg;
        if( impl->cb() )
        {
            try
            {
                impl->cb()->call();
            }
            catch (const std::exception& e)
            {
                std::cerr << "exception occured: " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "exception occured" << std::endl;
            }
        }

        return 0;
    }
}

namespace cxxtools {

void ThreadImpl::detach()
{
    if( _id )
    {
        int ret = pthread_detach(_id);
        if(ret != 0)
            throw SystemError( CXXTOOLS_ERROR_MSG("pthread_detach") );
    }
}


void ThreadImpl::init(const Callable<void>& cb)
{
    delete _cb;
    _cb = cb.clone();
}


void ThreadImpl::start()
{
    size_t stacksize = 0;

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    //pthread_attr_setinheritsched(&attrs, PTHREAD_INHERIT_SCHED);

    if(stacksize > 0)
        pthread_attr_setstacksize(&attrs ,stacksize);

    int ret = pthread_create(&_id, &attrs, thread_entry, this);
    pthread_attr_destroy(&attrs);

    if(ret != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_create") );
}


void ThreadImpl::join()
{
    void* threadRet = 0;
    int ret = pthread_join(_id, &threadRet);

    if(ret != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_join") );
}


void ThreadImpl::terminate()
{
    int ret = pthread_kill(_id, SIGKILL);
    if(ret != 0)
        throw SystemError( CXXTOOLS_ERROR_MSG("pthread_kill") );
}

}
