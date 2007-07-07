/*
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
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
