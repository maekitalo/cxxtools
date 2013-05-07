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
#include "cxxtools/textcodec.h"

#ifdef CXXTOOLS_WITH_STD_LOCALE

namespace std {

//
// codecvt facet for Char/char
//
std::locale::id codecvt<cxxtools::Char, char, cxxtools::MBState>::id;


codecvt<cxxtools::Char, char, cxxtools::MBState>::codecvt(size_t ref)
: locale::facet(ref)
{}


codecvt<cxxtools::Char, char, cxxtools::MBState>::~codecvt()
{}

//
// codecvt facet for char/char
//
std::locale::id codecvt<char, char, cxxtools::MBState>::id;


codecvt<char, char, cxxtools::MBState>::codecvt(size_t ref)
: locale::facet(ref)
{}


codecvt<char, char, cxxtools::MBState>::~codecvt()
{}

} // namespace std

#endif
