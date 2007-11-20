/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Duerner                          *
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
#include "cxxtools/atomicity.h"

#if defined (CXXTOOLS_ATOMICITY_WITH_PTHREAD)

#include <cassert>
#include <pthread.h>

namespace cxxtools {

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;


atomic_t atomicIncrement(volatile atomic_t& dest)
{
    atomic_t ret = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ++dest;
    ret = dest;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop (0);

    return (ret);
}


atomic_t atomicDecrement(volatile atomic_t& dest)
{
    atomic_t ret = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    --dest;
    ret = dest;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop (0);

    return (ret);
}


atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp)
{
    atomic_t old = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int ret = pthread_mutex_lock(&mtx);
    assert (ret == 0);

    old = dest;
    if(old == comp)
    {
        dest = exch;
    }

    ret = pthread_mutex_unlock(&mtx);
    assert (ret == 0);

    pthread_cleanup_pop (0);

    return old;
}


void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp)
{
    void* old = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int ret = pthread_mutex_lock(&mtx);
    assert(ret == 0);

    old = dest;
    if(old == comp)
    {
        dest = exch;
    }

    ret = pthread_mutex_unlock(&mtx);
    assert(ret == 0);

    pthread_cleanup_pop (0);

    return old;
}


atomic_t atomicExchange(volatile atomic_t& dest, atomic_t exch)
{
    atomic_t ret = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = dest;
    dest = exch;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop (0);

    return ret;
}



void* atomicExchange(void* volatile& dest, void* exch)
{
    void* ret = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert(thr_ret == 0);

    ret = dest;
    dest = exch;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert(thr_ret == 0);

    pthread_cleanup_pop (0);

    return ret;
}


atomic_t atomicExchangeAdd(volatile atomic_t& dest, atomic_t add)
{
    atomic_t ret = 0;

    pthread_cleanup_push ((void(*)(void *))pthread_mutex_unlock, (void *)&mtx);
    int thr_ret = pthread_mutex_lock(&mtx);
    assert (thr_ret == 0);

    ret = dest;
    dest += add;

    thr_ret = pthread_mutex_unlock(&mtx);
    assert (thr_ret == 0);

    pthread_cleanup_pop (0);

    return (ret);
}

} // namespace cxxtools

#else

#endif



