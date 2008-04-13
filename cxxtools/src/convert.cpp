/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
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
#include "cxxtools/convert.h"

namespace cxxtools
{


namespace
{
  std::string mkmessage(const std::type_info& to, const std::type_info& from)
  {
    std::ostringstream msg;
    msg << "conversion to " << to.name() << " from " << from.name() << " failed";
    return msg.str();
  }
}


ConversionError::ConversionError(const std::type_info& to, const std::type_info& from,
    const SourceInfo& si)
    : std::runtime_error(mkmessage(to, from)),
      _si(si),
      _to(to),
      _from(from)
{
}

}

