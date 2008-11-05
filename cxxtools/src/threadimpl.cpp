/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU Library   *
 *   General Public License.                                               *
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
#include "threadimpl.h"
#include "cxxtools/syserror.h"
#include <errno.h>

extern "C"
{
    static void* thread_entry(void* arg)
    {
        cxxtools::ThreadImpl* impl = (cxxtools::ThreadImpl*)arg;
        if( impl->cb() )
            impl->cb()->call();

        return 0;
    }
}

namespace
{
    void throwIf(int ret, pthread_t& id, const char* msg)
    {
        if(ret != 0)
        {
            id = 0;
            throw cxxtools::SysError(ret, msg);
        }
    }
}

namespace cxxtools {

void ThreadImpl::detach()
{
    if( _id )
    {
        int ret = pthread_detach(_id);
        throwIf(ret, _id, "Could not detach thread");
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

    throwIf(ret, _id, "Could not create thread");
}


void ThreadImpl::join()
{
    void* threadRet = 0;
    int ret = pthread_join(_id, &threadRet);

    throwIf(ret, _id, "Could not join thread");
}


void ThreadImpl::terminate()
{
    int ret = pthread_kill(_id, SIGKILL);

    throwIf(ret, _id, "Could not terminate thread");
}

}
