/*
 * Copyright (C) 2015 Tommi Maekitalo
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

#include <cxxtools/envsubst.h>
#include <cxxtools/log.h>
#include <stdlib.h>

log_define("cxxtools.envsubst")

namespace cxxtools
{

namespace
{
  bool isVarChar(char ch)
  {
    return (ch >= 'a' && ch <= 'z')
        || (ch >= 'A' && ch <= 'Z')
        || (ch >= '0' && ch <= '9')
        || (ch == '_');
  }
}

EnvSubstSyntaxError::EnvSubstSyntaxError(const std::string& msg)
  : std::runtime_error(msg)
  { }

void EnvSubst::parse(char ch)
{
  switch (_state)
  {
    case state_0:
        if (ch == '\\')
          _state = state_esc;
        else if (ch == '$')
          _state = state_varbegin;
        else
          _ev.onChar(ch);
        break;

    case state_esc:
        _ev.onChar(ch);
        _state = state_0;
        break;

    case state_varbegin:
        if (isVarChar(ch))
        {
          _varname = ch;
          _state = state_varname;
        }
        else if (ch == '{')
        {
          _varname.clear();
          _state = state_bvarname;
        }
        else
          throw EnvSubstSyntaxError("expected env variable");
        break;

    case state_varname:
        if (isVarChar(ch))
          _varname += ch;
        else
        {
          const char* e = ::getenv(_varname.c_str());
          if (e)
          {
            log_debug("envvar \"" << _varname << "\": " << e);
            while (*e)
              _ev.onChar(*e++);
          }
          else
            log_debug("envvar \"" << _varname << "\" is not set");

          if (ch == '\\')
            _state = state_esc;
          else
          {
            _ev.onChar(ch);
            _state = state_0;
          }
        }
        break;

    case state_bvarname:
        if (ch == '}')
        {
          const char* e = ::getenv(_varname.c_str());
          if (e)
          {
            log_debug("envvar \"" << _varname << "\": " << e);
            while (*e)
              _ev.onChar(*e++);
          }
          else
            log_debug("envvar \"" << _varname << "\" is not set");

          _state = state_0;
        }
        else if (ch == ':')
        {
          _state = state_subst0;
        }
        else
          _varname += ch;
        break;

    case state_subst0:
        if (ch == '-')
        {
          const char* e = ::getenv(_varname.c_str());
          if (e)
          {
            log_debug("envvar \"" << _varname << "\": " << e);
            while (*e)
              _ev.onChar(*e++);
            _state = state_nosubst;
          }
          else
          {
            log_debug("envvar \"" << _varname << "\" is not set");
            _next = new EnvSubst(_ev);
            _state = state_subst;
          }
        }
        else
          throw EnvSubstSyntaxError(std::string("invalid substitution operaror ") + ch);
        break;

    case state_subst:
        if (ch == '}' && !_next->isInBracket())
        {
          _next->parseEnd();
          delete _next;
          _next = 0;
          _state = state_0;
        }
        else
          _next->parse(ch);
        break;

    case state_nosubst:
        if (ch == '}')
          _state = state_0;
        break;
  }

}

void EnvSubst::parseEnd()
{
  switch (_state)
  {
    case state_0:
        break;

    case state_esc:
    case state_varbegin:
    case state_bvarname:
    case state_subst0:
    case state_subst:
    case state_nosubst:
        throw EnvSubstSyntaxError("unexpected end");

    case state_varname:
        const char* e = ::getenv(_varname.c_str());
        if (e)
        {
          log_debug("envvar \"" << _varname << "\": " << e);
          while (*e)
            _ev.onChar(*e++);
        }
        else
          log_debug("envvar \"" << _varname << "\" is not set");

        _state = state_0;
        break;
  }

}

std::string envSubst(const std::string& str)
{
  struct Ev : public EnvSubst::Ev
  {
    std::string ret;
    void onChar(char ch)
    {
      ret += ch;
    }
  } ev;

  log_debug("envSubst(\"" << str << '"');

  EnvSubst es(ev);
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    es.parse(*it);
  es.parseEnd();

  log_debug("envSubst => \"" << ev.ret << '"');

  return ev.ret;
}


}
