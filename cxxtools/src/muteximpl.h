/*
 * Copyright (C) 2005 - 2007 by Marc Boris Duerner
 * Copyright (C) 2005 - 2007 by Aloysius Indrayanto
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
#ifndef CXXTOOLS_MUTEXIMPL_H
#define CXXTOOLS_MUTEXIMPL_H

#include <pthread.h>

namespace cxxtools {

class MutexImpl
{
    public:
        explicit MutexImpl();

        MutexImpl(int n);

        ~MutexImpl();

        void lock();

        bool tryLock();

        void unlock();

        pthread_mutex_t* handle()
        { return &_handle; }

        const pthread_mutex_t* handle() const
        { return &_handle; }

    private:
        pthread_mutex_t _handle;
};


class ReadWriteMutexImpl
{
    public:
        ReadWriteMutexImpl();

        ~ReadWriteMutexImpl();

        void readLock();

        bool tryReadLock();

        void writeLock();

        bool tryWriteLock();

        void unlock();

    private:
        pthread_rwlock_t _rwl;
};

} // !namespace cxxtools

#endif // CXXTOOLS_MUTEXIMPL_H
