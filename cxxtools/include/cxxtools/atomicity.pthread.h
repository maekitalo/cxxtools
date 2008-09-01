/***************************************************************************
 *   Copyright (C) 2006-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
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
#ifndef CXXTOOLS_ATOMICINT_PTHREAD_H
#define CXXTOOLS_ATOMICINT_PTHREAD_H

#include <csignal>

namespace cxxtools {

typedef std::sig_atomic_t atomic_t;

atomic_t atomicGet(volatile atomic_t& val);

void atomicSet(volatile atomic_t& val, atomic_t n);

atomic_t atomicIncrement(volatile atomic_t& val);

atomic_t atomicDecrement(volatile atomic_t& dest);

atomic_t atomicCompareExchange(volatile atomic_t& dest, atomic_t exch, atomic_t comp);

void* atomicCompareExchange(void* volatile& dest, void* exch, void* comp);

atomic_t atomicExchange(volatile atomic_t& dest, atomic_t exch);

void* atomicExchange(void* volatile& dest, void* exch);

atomic_t atomicExchangeAdd(volatile atomic_t& dest, atomic_t add);

} // namespace cxxtools

#endif
