/* cxxtools/trace.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <iostream>

class CTrace
{
    const char* m_msg;
    bool TraceFlag;

  public
    CTrace(const char* msg)
      : m_msg(msg)
    {
      if (TraceFlag)
        std::cerr << "start: " << msg << std::endl;
    }
    ~CTrace()
    {
      if (TraceFlag)
        std::cerr << "end  : " << msg << std::endl;
    }

    static setTrace(bool flag)
    { TraceFlag = flag; }
};
