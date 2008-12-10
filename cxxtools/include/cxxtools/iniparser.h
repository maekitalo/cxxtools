/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <iosfwd>
#include <string>

namespace cxxtools
{
  /**
   * Parser for files in ini-format
   */
  class IniParser
  {
    public:
      class Event
      {
        public:
          // events return true, if parsing should be stopped
          virtual bool onSection(const std::string& section);
          virtual bool onKey(const std::string& key);
          virtual bool onValue(const std::string& key);
          virtual bool onComment(const std::string& comment);
          virtual bool onError();
      };

    private:
      Event& event;
      std::string data;
      enum
      {
        state_0,
        state_section,
        state_key,
        state_key_sp,
        state_value0,
        state_value,
        state_comment

      } state;

    public:
      IniParser(Event& event_)
        : event(event_),
          state(state_0)
        { }

      bool parse(char ch);
      void end();
      void parse(std::istream& in);
  };
}
