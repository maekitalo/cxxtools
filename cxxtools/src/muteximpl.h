/***************************************************************************
 *   Copyright (C) 2005 - 2007 by Marc Boris Duerner                       *
 *   Copyright (C) 2005 - 2007 by Aloysius Indrayanto                      *
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
