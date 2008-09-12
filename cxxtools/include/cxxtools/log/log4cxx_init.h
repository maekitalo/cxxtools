/* cxxtools/log/log4cxx_init.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2004 Tommi Maekitalo
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
 *
 */

#ifndef CXXTOOLS_LOG_LOG4CXX_INIT_H
#define CXXTOOLS_LOG_LOG4CXX_INIT_H

#include <cxxtools/log/log4cxx.h>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logmanager.h>

#define log_init_fatal()   log_init(::log4cxx::Level::FATAL)
#define log_init_error()   log_init(::log4cxx::Level::ERROR)
#define log_init_warn()    log_init(::log4cxx::Level::WARN)
#define log_init_info()    log_init(::log4cxx::Level::INFO)
#define log_init_debug()   log_init(::log4cxx::Level::DEBUG)
#define log_init_trace()   log_init(::log4cxx::Level::DEBUG)

inline void log_init(const std::string& configFileName)
{
#ifdef LOG4CXX_HAVE_XML
  if (log4cxx::helpers::StringHelper::endsWith(configFileName, ".xml"))
  {
    ::log4cxx::xml::DOMConfigurator::configure(configFileName);
  }
  else
#endif
  {
    ::log4cxx::PropertyConfigurator::configure(configFileName);
  }
}

inline void log_init(const char* filename)
{
  log_init(std::string(filename));
}

inline void log_init()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init(LOGPROPERTIES);
  else
  {
    try
    {
      log_init("log4j.properties");
    }
    catch (const log4cxx::helpers::IOException&)
    {
      log4cxx::BasicConfigurator::configure();
    }
  }
}

inline void log_init(log4cxx::LevelPtr level)
{
  log4cxx::BasicConfigurator::configure();
  log4cxx::LoggerPtr rootLogger = log4cxx::Logger::getRootLogger();
  rootLogger->setLevel(level);
}

#endif // CXXTOOLS_LOG_LOG4CXX_INIT_H
